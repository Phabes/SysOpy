#include <unistd.h>
#include <ctype.h>

#include "common.h"

int client_q, server_q;
int client_id = -1;

void close_client(){
    msgctl(client_q, IPC_RMID, NULL);
    exit(0);
}

void sigint_handler(){
    printf("\n");
    message_buf msg;
    msg.client_q = -1;
    msg.client_id = client_id;
    msg.type = STOP;
    send_message(server_q, &msg);
    close_client();
}

int check_input(){
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(0, &fds);
}

int is_number(char *number){
    int length = strlen(number);
    for(int i = 0; i < length; i++){
        if(isdigit(number[i]) == 0)
            return 0;
    }
    return 1;
}

void execute_command(char line[MAX_LENGTH], message_buf *msg){
    char *element;
    element = strtok(line, " ");
    if(strcmp(line, "STOP\n") == 0){
        msg->client_q = -1;
        msg->client_id = client_id;
        msg->type = STOP;
        send_message(server_q, msg);
        close_client();
    }
    else if(strcmp(line, "LIST\n") == 0){
        msg->client_q = -1;
        msg->client_id = client_id;
        msg->type = LIST;
        send_message(server_q, msg);
    }
    else if(strcmp(element, "2ALL") == 0){
        msg->client_q = -1;
        msg->client_id = client_id;
        msg->type = TO_ALL;
        element = strtok(NULL, "");
        element[strlen(element) - 1] = '\0';
        strcpy(msg->text, element);
        send_message(server_q, msg);
    }
    else if(strcmp(element, "2ONE") == 0){
        element = strtok(NULL, " ");
        if(is_number(element) == 1){
            msg->client_id = client_id;
            msg->type = TO_ONE;
            msg->client_q = atoi(element);
            element = strtok(NULL, "");
            element[strlen(element) - 1] = '\0';
            strcpy(msg->text, element);
            send_message(server_q, msg);
        }
        else
            printf("WRONG CLIENT ID\n");
    }
    printf("\n");
}

void msg_back_handler(message_buf *msg_back){
    if(msg_back->type == STOP){
        sigint_handler();
    }
    else if(msg_back->type == LIST){
        printf("%s\n", msg_back->text);
    }
    else if(msg_back->type == TO_ALL){
        struct tm* current_time = localtime(&msg_back->time);
        printf("FROM: %d\n", msg_back->client_id);
        printf("TIME: %02d/%02d/%d %02d:%02d:%02d\n", current_time->tm_mday, current_time->tm_mon + 1, current_time->tm_year + 1900, current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
        printf("MESSAGE: %s\n\n", msg_back->text);
    }
    else if(msg_back->type == TO_ONE){
        struct tm* current_time = localtime(&msg_back->time);
        printf("FROM: %d\n", msg_back->client_id);
        printf("TIME: %02d/%02d/%d %02d:%02d:%02d\n", current_time->tm_mday, current_time->tm_mon + 1, current_time->tm_year + 1900, current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
        printf("MESSAGE: %s\n\n", msg_back->text);
    }
    else if(msg_back->type == INIT){
        if(msg_back->client_id != -1){
            client_id = msg_back->client_id;
            printf("%s\n", msg_back->text);
        }
        else{
            printf("SERVER IS FULL\n");
            exit(0);
        }
    }
}

int main(int argc, char *argv[]){    
    key_t server_key = ftok(HOME, ID);
    server_q = msgget(server_key, 0);
    printf("SERVER KEY: %d\n", server_key);
    printf("SERVER IPC: %d\n", server_q);

    key_t client_key = ftok(HOME, getpid());
    client_q = msgget(client_key, FLAGS);
    printf("CLIENT KEY: %d\n", client_key);
    printf("CLIENT IPC: %d\n\n", client_q);

    message_buf msg;
    msg.type = INIT;
    msg.client_q = client_q;
    msg.client_id = -1;
    send_message(server_q, &msg);

    signal(SIGINT, &sigint_handler);

    message_buf msg_back;
    char line[MAX_LENGTH];
    while (1){
        if(check_input() != 0){
            if(fgets (line, MAX_LENGTH, stdin))
                execute_command(line, &msg);
        }
        if(get_message_nowait(client_q, &msg_back) != -1)
            msg_back_handler(&msg_back);
        // sleep(1);
    }
    return 0;
}