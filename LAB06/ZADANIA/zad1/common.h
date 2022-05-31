#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/msg.h>
#include <time.h>

#ifndef DATA_H
#define DATA_H

#define MAX_LENGTH 256
#define MAX_CLIENTS 20
#define HOME getenv("HOME")
#define ID 'W'
#define FLAGS IPC_CREAT | 0666

#define STOP 1
#define LIST 2
#define TO_ALL 3
#define TO_ONE 4
#define INIT 5

typedef struct {
	long type;
	char text[MAX_LENGTH];
    int client_q;
    int client_id;
    time_t time;
} message_buf;

typedef struct {
    int client_q;
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

void send_message(int to, message_buf *msg){
    msgsnd(to, msg, sizeof(message_buf) - sizeof(long), 0);
}

int get_message(int to, message_buf *msg){
    return msgrcv(to, msg, sizeof(message_buf) - sizeof(long), 0, 0);
}

int get_message_nowait(int to, message_buf *msg){
    return msgrcv(to, msg, sizeof(message_buf) - sizeof(long), 0, IPC_NOWAIT);
}

#endif //DATA_H