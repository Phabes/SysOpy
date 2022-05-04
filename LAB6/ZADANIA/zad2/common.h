#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <mqueue.h>

#ifndef DATA_H
#define DATA_H

#define SERVER_Q "/server_q"
#define MAX_MSG 10
#define MAX_LENGTH 256
#define MAX_CLIENTS 20
#define MODE 0666

#define STOP 1
#define LIST 2
#define TO_ALL 3
#define TO_ONE 4
#define INIT 5

typedef struct {
	long type;
	char text[MAX_LENGTH];
    mqd_t client_q;
    int client_id;
    time_t time;
} message_buf;

typedef struct {
    mqd_t client_q;
    int available;
    int id;
} client;

char *get_command_name(long type){
    if(type == STOP)
        return "STOP";
    if(type == LIST)
        return "LIST";
    if(type == TO_ALL)
        return "2ALL";
    if(type == TO_ONE)
        return "2ONE";
    if(type == INIT)
        return "INIT";
    return "WRONG_ARG";
}

void send_message(mqd_t descriptor, message_buf *msg){
    mq_send(descriptor, (char *) msg, sizeof(message_buf), msg->type);
}

ssize_t get_message(mqd_t descriptor, message_buf *msg, unsigned int *type){
    return mq_receive(descriptor, (char *) msg, sizeof(message_buf), type);
}

char *get_client_name(){
    char *name = calloc(MAX_LENGTH, sizeof(char));
    sprintf(name, "/client_q_%d", getpid());
    return name;
}

#endif //DATA_H