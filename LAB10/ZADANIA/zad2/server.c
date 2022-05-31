#include "common.h"

#define MAX_CLIENTS 10

typedef struct{
    char *name;
    int available;
    int opponent;
    int online;
    int fd;
    struct sockaddr addr;
} client;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
client *clients[MAX_CLIENTS] = {};
int connected_clients = 0;

int get_random_number(int lower, int upper){
    return rand() % (upper - lower + 1) + lower;
}

int get_client_descriptor(int local_socket, int web_socket){
    struct pollfd fds[2];
    fds[0].fd = local_socket;
    fds[0].events = POLLIN;
    fds[1].fd = web_socket;
    fds[1].events = POLLIN;
    poll(fds, 2, -1);
    for (int i = 0; i < 2; i++){
        if (fds[i].revents & POLLIN)
            return fds[i].fd;
    }
    printf("NIII\n");
    return -1;
}

int find_empty_slot(){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i] == NULL)
            return i;
    }
    return -1;
}

int find_by_name(char *name){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i] != NULL){
            if(strcmp(clients[i]->name, name) == 0)
                return i;
        }
    }
    return -1;
}

int find_opponent(int index){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(index != i && clients[i] != NULL){
            if(clients[i]->available == 1)
                return i;
        }
    }
    return -1;
}

void remove_client(int index){
    free(clients[index]->name);
    free(clients[index]);
    clients[index] = NULL;
    connected_clients--;
}

void remove_both_clients(int index){
    int opponent = clients[index]->opponent;
    remove_client(index);
    if(clients[opponent] != NULL){
        char buff[MESS_LENGTH];
        sprintf(buff, "quit:%s", clients[opponent]->name);
        sendto(clients[opponent]->fd, buff, MESS_LENGTH, 0, &clients[opponent]->addr, sizeof(struct sockaddr));
        remove_client(opponent);
    }
}

void close_server(){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i] != NULL){
            char buff[MESS_LENGTH];
            sprintf(buff, "quit:%s", clients[i]->name);
            sendto(clients[i]->fd, buff, MESS_LENGTH, 0, &clients[i]->addr, sizeof(struct sockaddr));
            remove_client(i);
        }
    }
    exit(0);
}

void delete_offline_players(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] != NULL && clients[i]->online == 0)
            remove_both_clients(i);
    }
}

void send_ping_to_all_clients(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] != NULL){
            sendto(clients[i]->fd, "ping: ", MESS_LENGTH, 0, &clients[i]->addr, sizeof(struct sockaddr));
            clients[i]->online = 0;
        }
    }
}

void ping(){
    while(1){
        pthread_mutex_lock(&mutex);
        delete_offline_players();
        send_ping_to_all_clients();
        pthread_mutex_unlock(&mutex);
        sleep(5);
    }
}

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("WRONG NUMBER OF ARGUMENTS\n");
        exit(0);
    }
    int port = atoi(argv[1]);
    char *socket_path = argv[2];

    int local_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(local_socket == -1){
        printf("LOCAL SOCKET CREATE ERROR\n");
        exit(0);
    }
    struct sockaddr_un local_addr;
    local_addr.sun_family = AF_UNIX;
    strcpy(local_addr.sun_path, socket_path);
    int local_bind = bind(local_socket, (struct sockaddr *) &local_addr, sizeof(struct sockaddr_un));
    if(local_bind == -1){
        printf("LOCAL BIND ERROR\n");
        exit(0);
    }

    int web_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(web_socket == -1){
        printf("WEB SOCKET CREATE ERROR\n");
        exit(0);
    }
    struct sockaddr_in web_addr;
    web_addr.sin_family = AF_INET;
    web_addr.sin_port = htons(port);
    web_addr.sin_addr.s_addr = INADDR_ANY;
    int web_bind = bind(web_socket, (struct sockaddr *) &web_addr, sizeof(struct sockaddr_in));
    if(web_bind == -1){
        printf("WEB BIND ERROR\n");
        exit(0);
    }

    signal(SIGINT, &close_server);
    pthread_t t;
    pthread_create(&t, NULL, (void *(*)(void *))ping, NULL);
    while(1){
        int client_fd = get_client_descriptor(local_socket, web_socket);
        char buff[MESS_LENGTH];
        socklen_t length = sizeof(struct sockaddr);
        struct sockaddr addr;
        recvfrom(client_fd, buff, MESS_LENGTH, 0, &addr, &length);
        printf("BUFF: %s\n", buff);
        char *command = strtok(buff, ":");
        char *arg = strtok(NULL, ":");
        char *name = strtok(NULL, ":");
        pthread_mutex_lock(&mutex);
        if(strcmp(command, "init") == 0){
            if(find_by_name(name) == -1){
                int client_index = find_empty_slot();
                if(client_index == -1)
                    sendto(client_fd, "init:no_slot", MESS_LENGTH, 0, &addr, sizeof(struct addrinfo));
                else{
                    client *new_client = malloc(sizeof(client));
                    new_client->name = calloc(MESS_LENGTH, sizeof(char));
                    strcpy(new_client->name, name);
                    new_client->available = 1;
                    new_client->opponent = -1;
                    new_client->online = 1;
                    new_client->fd = client_fd;
                    new_client->addr = addr;
                    clients[client_index] = new_client;
                    connected_clients++;
                    int opponent = find_opponent(client_index);
                    if(opponent == -1)
                        sendto(client_fd, "init:no_enemy", MESS_LENGTH, 0, (struct sockaddr *) &addr, sizeof(struct addrinfo));
                    else{
                        int number = get_random_number(0, 1);
                        clients[client_index]->available = 0;
                        clients[opponent]->available = 0;
                        clients[client_index]->opponent = opponent;
                        clients[opponent]->opponent = client_index;
                        if(number % 2 == 0){
                            sendto(clients[client_index]->fd, "init:O", MESS_LENGTH, 0, &clients[client_index]->addr, sizeof(struct addrinfo));
                            sendto(clients[opponent]->fd, "init:X", MESS_LENGTH, 0, &clients[opponent]->addr, sizeof(struct addrinfo));
                        }
                        else{
                            sendto(clients[opponent]->fd, "init:O", MESS_LENGTH, 0, &clients[opponent]->addr, sizeof(struct addrinfo));
                            sendto(clients[client_index]->fd, "init:X", MESS_LENGTH, 0, &clients[client_index]->addr, sizeof(struct addrinfo));
                        }
                    }
                }
            }
            else
                sendto(client_fd, "init:taken", MESS_LENGTH, 0, (struct sockaddr *) &addr, sizeof(struct addrinfo));
        }
        else if(strcmp(command, "move") == 0){
            int move = atoi(arg);
            int player = find_by_name(name);
            int opponent = clients[player]->opponent;
            sprintf(buff, "opponent_move:%d", move);
            sendto(clients[opponent]->fd, buff, MESS_LENGTH, 0, &clients[opponent]->addr, sizeof(struct addrinfo));
        }
        else if(strcmp(command, "quit") == 0){
            int player = find_by_name(name);
            remove_both_clients(player);
        }
        else if(strcmp(command, "exit") == 0){
            int player = find_by_name(name);
            remove_client(player);
        }
        else if(strcmp(command, "ping") == 0){
            int player = find_by_name(name);
            clients[player]->online = 1;
        }
        pthread_mutex_unlock(&mutex);
    }
    return 0;
}