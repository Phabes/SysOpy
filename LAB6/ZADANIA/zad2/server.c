#include "common.h"

client clients[MAX_CLIENTS];

mqd_t server_q;
int running = 1;

void close_server(){
    mq_close(server_q);
    mq_unlink(SERVER_Q);
    exit(0);
}

int check_clients_close(){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i].available == 0)
            return 0;
    }
    return 1;
}

void sigint_handler(){
    printf("\n");
    running = 0;
    message_buf msg;
    msg.type = STOP;
    if(check_clients_close() == 1)
        close_server();
    else{
        for(int i = 0; i < MAX_CLIENTS; i++){
            if(clients[i].available == 0)
                send_message(clients[i].client_q, &msg);
        }
    }
}

void create_clients_structure(){
    for(int i = 0; i < MAX_CLIENTS; i++)
        clients[i].available = 1;
}

int find_available(){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i].available == 1)
            return i;
    }
    return -1;
}

void create_log(message_buf *msg, time_t curr_time){
    struct tm* current_time = localtime(&curr_time);
    FILE *log_file = fopen("logs.txt", "a");
    if(log_file != NULL){
        char date_str[20];
        sprintf(date_str, "%02d/%02d/%d %02d:%02d:%02d", current_time->tm_mday, current_time->tm_mon + 1, current_time->tm_year + 1900, current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
        fprintf(log_file, "%15s\t%15d\t%30s\n", get_command_name(msg->type), msg->client_id, date_str);
        fclose(log_file);
    }
}

void handle_msg(message_buf *msg){
    message_buf msg_back;
    time_t curr_time = time(NULL);
    create_log(msg, curr_time);
    msg_back.text[0] = '\0';
    if(msg->type == STOP){
        printf("STOP: %d\n", msg->client_id);
        clients[msg->client_id].available = 1;
        mq_close(clients[msg->client_id].client_q);
        if(running == 0){
            if(check_clients_close() == 1)
                close_server();
        }
    }
    else if(msg->type == LIST){
        printf("LIST: %d\n", msg->client_id);
        msg_back.type = LIST;
        strcat(msg_back.text, "CLIENTS LIST\n");
        for(int i = 0; i < MAX_CLIENTS; i++){
            if(msg->client_id != i && clients[i].available == 0){
                strcat(msg_back.text, "ID: ");
                char index_str[4];
                sprintf(index_str, "%d\n", i);
                strcat(msg_back.text, index_str);
            }
        }
        strcat(msg_back.text, "END OF LIST\n");
        send_message(clients[msg->client_id].client_q, &msg_back);
    }
    else if(msg->type == TO_ALL){
        printf("2ALL: %d\n", msg->client_id);
        msg_back.client_id = msg->client_id;
        msg_back.type = TO_ALL;
        strcpy(msg_back.text, msg->text);
        msg_back.time = curr_time;
        for(int i = 0; i < MAX_CLIENTS; i++){
            if(msg->client_id != i && clients[i].available == 0)
                send_message(clients[i].client_q, &msg_back);
        }
    }
    else if(msg->type == TO_ONE){
        printf("2ONE: %d\n", msg->client_id);
        msg_back.client_id = msg->client_id;
        msg_back.type = TO_ONE;
        strcpy(msg_back.text, msg->text);
        msg_back.time = curr_time;
        if(msg->client_q < MAX_CLIENTS && clients[msg->client_q].available == 0)
            send_message(clients[msg->client_q].client_q, &msg_back);
    }
    else if(msg->type == INIT){
        int index = find_available();
        msg_back.type = INIT;
        msg_back.client_id = index;
        mqd_t client_queue = mq_open(msg->text, O_WRONLY);
        if(index != -1){
            printf("INIT: %d\n", index);
            clients[index].available = 0;
            clients[index].client_q = mq_open(msg->text, O_WRONLY);
            clients[index].id = index;
            strcat(msg_back.text, "GIVEN ID: ");
            char index_str[4];
            sprintf(index_str, "%d\n", index);
            strcat(msg_back.text, index_str);
        }
        else
            printf("SERVER IF FULL\n");
        send_message(client_queue, &msg_back);
    }
}

int main(int argc, char *argv[]){
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MSG;
    attr.mq_msgsize = sizeof(message_buf);
    attr.mq_curmsgs = 0;

    server_q = mq_open(SERVER_Q, O_RDONLY | O_CREAT | O_EXCL, MODE, &attr);
    printf("SERVER IPC: %d\n", server_q);

    signal(SIGINT, &sigint_handler);

    create_clients_structure();
    message_buf msg;
    unsigned int type;
    while(1){
        if(get_message(server_q, &msg, &type) != -1)
            handle_msg(&msg);
        // sleep(1);
    }
    return 0;
}