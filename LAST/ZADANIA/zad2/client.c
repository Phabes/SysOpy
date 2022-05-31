#include "common.h"

typedef enum{
    GAME_STARTING,
    WAITING,
    WAITING_FOR_MOVE,
    OPPONENT_MOVE,
    MOVE,
    GAME_OVER
} state;

typedef enum{
    EMPTY,
    O,
    X
} square;

typedef struct{
    square squares[9];
} board;

char *name;
char *arg;
int server_socket;
square client_sign;
state game_state = GAME_STARTING;
board game_board;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int connect_local(char *path){
    int server_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(server_socket == -1){
        printf("LOCAL SOCKET CREATE ERROR\n");
        exit(0);
    }
    struct sockaddr_un binded_sockaddr;
    binded_sockaddr.sun_family = AF_UNIX;
    sprintf(binded_sockaddr.sun_path, "%d", getpid());
    int local_bind = bind(server_socket, (struct sockaddr *) &binded_sockaddr, sizeof(struct sockaddr_un));
    if(local_bind == -1){
        printf("LOCAL BIND ERROR\n");
        exit(0);
    }
    struct sockaddr_un local_addr;
    local_addr.sun_family = AF_UNIX;
    strcpy(local_addr.sun_path, path);
    int client_connection = connect(server_socket, (struct sockaddr *) &local_addr, sizeof(struct sockaddr_un));
    if(client_connection == -1){
        printf("LOCAL CONNECTION ERROR\n");
        exit(0);
    }
    char msg[MESS_LENGTH];
    sprintf(msg, "init: :%s", name);
    sendto(server_socket, msg, MESS_LENGTH, 0, (struct sockaddr *) &local_addr, sizeof(struct sockaddr_un));
    return server_socket;
}

int connect_inet(int port){
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(server_socket == -1){
        printf("WEB SOCKET CREATE ERROR\n");
        exit(0);
    }
    struct sockaddr_in web_addr;
    web_addr.sin_family = AF_INET;
    web_addr.sin_port = htons(port);
    web_addr.sin_addr.s_addr = INADDR_ANY;
    int client_connection = connect(server_socket, (struct sockaddr *) &web_addr, sizeof(struct sockaddr_in));
    if(client_connection == -1){
        printf("WEB CONNECTION ERROR\n");
        exit(0);
    }
    char msg[MESS_LENGTH];
    sprintf(msg, "init: :%s", name);
    sendto(server_socket, msg, MESS_LENGTH, 0, (struct sockaddr *) &web_addr, sizeof(struct sockaddr_in));
    return server_socket;
}

board create_new_board(){
    board new_board = {{EMPTY}};
    return new_board;
}

void draw_board(){
    for(int i = 0; i < 7; i++)
        printf("-");
    for(int i = 0; i < 3; i++){
        printf("\n");
        for(int j = 0; j < 3; j++){
            printf("|");
            if(game_board.squares[3 * i + j] == X)
                printf("X");
            else if(game_board.squares[3 * i + j] == O)
                printf("O");
            else
                printf(" ");
        }
        printf("|\n");
        for(int j = 0; j < 7; j++)
            printf("-");
    }
    printf("\n");
}

int check_move(int move){
    if(move < 0 || move > 8)
        return 0;
    if(game_board.squares[move] == EMPTY)
        return 1;
    return 0;
}

int check_end(square sign){
    for(int i = 0; i < 3; i++){
        int row_win = 1;
        for(int j = 0; j < 3; j++){
            if(game_board.squares[i * 3 + j] != sign){
                row_win = 0;
                break;
            }
        }
        if(row_win == 1)
            return 1;
    }
    for(int j = 0; j < 3; j++){
        int column_win = 1;
        for(int i = 0; i < 3; i++){
            if(game_board.squares[i * 3 + j] != sign){
                column_win = 0;
                break;
            }
        }
        if(column_win == 1)
            return 1;
    }
    int cross_win = 1;
    for(int i = 0; i < 9; i += 4){
        if(game_board.squares[i] != sign){
            cross_win = 0;
            break;
        }
    }
    if(cross_win == 1)
        return 1;
    cross_win = 1;
    for(int i = 2; i < 7; i += 2){
        if(game_board.squares[i] != sign){
            cross_win = 0;
            break;
        }
    }
    if(cross_win == 1)
        return 1;
    for(int i = 0; i < 9; i++){
        if(game_board.squares[i] == EMPTY)
            return 0;
    }
    return -1;
}

void close_connection(){
    int closed = close(server_socket);
    if(closed == -1){
        printf("SERVER SOCKET CLOSE ERROR\n");
        exit(0);
    }
    exit(0);
}

void quit(){
    char buff[MESS_LENGTH];
    sprintf(buff, "quit: :%s", name);
    send(server_socket, buff, MESS_LENGTH, 0);
    close_connection();
}

void after_game(){
    char buff[MESS_LENGTH];
    sprintf(buff, "exit: :%s", name);
    send(server_socket, buff, MESS_LENGTH, 0);
    close_connection();
}

square get_other_sign(square sign){
    return sign == O ? X : O;
}

void play_game(){
    while (1){
        if (game_state == GAME_STARTING){
            if (strcmp(arg, "taken") == 0){
                printf("NAME ALREADY TAKEN\n");
                exit(0);
            }
            else if (strcmp(arg, "no_enemy") == 0){
                printf("WAITING FOR OPPONENT\n");
                game_state = WAITING;
            }
            else{
                game_board = create_new_board();
                if(arg[0] == 'O'){
                    client_sign = O;
                    game_state = MOVE;
                }
                else{
                    client_sign = X;
                    game_state = WAITING_FOR_MOVE;
                }
                draw_board();
            }
        }
        else if (game_state == WAITING){
            pthread_mutex_lock(&mutex);
            while (game_state != GAME_STARTING && game_state != GAME_OVER)
                pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);
            game_board = create_new_board();
            if(arg[0] == 'O'){
                client_sign = O;
                game_state = MOVE;
            }
            else{
                client_sign = X;
                game_state = WAITING_FOR_MOVE;
            }
            draw_board();
        }
        else if (game_state == WAITING_FOR_MOVE){
            printf("WAITING FOR OPPONENT MOVE\n");
            pthread_mutex_lock(&mutex);
            while (game_state != OPPONENT_MOVE && game_state != GAME_OVER)
                pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);
        }
        else if (game_state == MOVE){
            int move;
            do{
                printf("YOUR MOVE (%c): ", client_sign == O ? 'O' : 'X');
                scanf("%d", &move);
                move--;
            } while (check_move(move) == 0);
            game_board.squares[move] = client_sign;
            draw_board();
            char buff[MESS_LENGTH];
            sprintf(buff, "move:%d:%s", move, name);
            send(server_socket, buff, MESS_LENGTH, 0);
            int end = check_end(client_sign);
            if(end != 0){
                if(end == 1)
                    printf("YOU WIN\n");
                else
                    printf("DRAW\n");
                game_state = GAME_OVER;
            }
            if (game_state != GAME_OVER)
                game_state = WAITING_FOR_MOVE;
        }
        else if (game_state == OPPONENT_MOVE){
            int move = atoi(arg);
            square sign = get_other_sign(client_sign);
            game_board.squares[move] = sign;
            draw_board();
            int end = check_end(sign);
            if(end != 0){
                if(end == 1)
                    printf("YOU LOST\n");
                else
                    printf("DRAW\n");
                game_state = GAME_OVER;
            }
            if (game_state != GAME_OVER)
                game_state = MOVE;
        }
        else if (game_state == GAME_OVER)
            after_game();
    }
}

void ping(){
    char buff[MESS_LENGTH];
    sprintf(buff, "ping: :%s", name);
    send(server_socket, buff, MESS_LENGTH, 0);
}

int main(int argc, char *argv[]){
    if(argc != 4){
        printf("WRONG NUMBER OF ARGUMENTS\n");
        exit(0);
    }
    name = argv[1];
    if(strcmp(argv[2], "unix") == 0){
        char *path = argv[3];
        server_socket = connect_local(path);
    }
    else if(strcmp(argv[2], "inet") == 0){
        int port = atoi(argv[3]);
        server_socket = connect_inet(port);
    }
    else{
        printf("WRONG TYPE. AVAILABLE OPTIONS: unix and inet\n");
        exit(0);
    }
    char buff[MESS_LENGTH];
    char *command;
    int game_thread_running = 0;
    signal(SIGINT, &quit);
    while(1){
        recv(server_socket, buff, MESS_LENGTH, 0);
        command = strtok(buff, ":");
        arg = strtok(NULL, ":");
        pthread_mutex_lock(&mutex);
        if(strcmp(command, "init") == 0){
            game_state = GAME_STARTING;
            if (game_thread_running == 0){
                pthread_t t;
                int thread = pthread_create(&t, NULL, (void *(*)(void *))play_game, NULL);
                if(thread != 0){
                    printf("THREAD CREATION ERROR\n");
                    exit(0);
                }
                game_thread_running = 1;
            }
        }
        else if(strcmp(command, "opponent_move") == 0)
            game_state = OPPONENT_MOVE;
        else if(strcmp(command, "quit") == 0){
            game_state = GAME_OVER;
            close_connection();
        }
        else if(strcmp(command, "ping") == 0)
            ping();
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return 0;
}