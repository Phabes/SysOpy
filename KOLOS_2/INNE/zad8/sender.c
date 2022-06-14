#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#define KEY "/home/ajris"

struct mess {
    long mtype;
    long value;
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Not a suitable number of program parameters\n");
        return (1);
    }
    /******************************************************
    Utworz kolejke komunikatow systemu V "reprezentowana" przez KEY
    Wyslij do niej wartosc przekazana jako parametr wywolania programu
    Obowiazuja funkcje systemu V
    ******************************************************/
    key_t key = ftok(KEY, 'q');
    int queue = msgget(key, IPC_CREAT | 0666);
    if(queue == -1){
        printf("MSGGET ERROR\n");
        exit(0);
    }
    struct mess buf;
    buf.mtype = 1;
    buf.value = atoi(argv[1]);
    int snd = msgsnd(queue, &buf, sizeof(struct mess) - sizeof(long), 0);
    if(snd == -1){
        printf("MSGSND ERROR\n");
        exit(0);
    }
    return 0;
}