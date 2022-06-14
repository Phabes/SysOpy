#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define KEY  "/home/ajris"

struct mess {
    long mtype;
    long value;
};


int main() {
    sleep(1);
    /**********************************
    Otworz kolejke systemu V "reprezentowana" przez KEY
    **********************************/
    key_t key = ftok(KEY, 'q');
    int queue = msgget(key, 0666);
    if(queue == -1){
        printf("MSGGET ERROR\n");
        exit(0);
    }

    /**********************************
    Odczytaj zapisane w kolejce wartosci i przypisz je do zmiennej val
    obowiazuja funkcje systemu V
    ************************************/
    struct mess buf;
    int rcv = msgrcv(queue, &buf, sizeof(struct mess) - sizeof(long), 0, 0);
    if(rcv == -1){
        printf("MSGRCV ERROR\n");
        exit(0);
    }
    int val = buf.value;

    printf("%d square is: %d\n", val, val * val);
    /*******************************
    posprzataj
    ********************************/
    int ctl = msgctl(queue, IPC_RMID, NULL);
    if(ctl == -1){
        printf("MSGCTL ERROR\n");
        exit(0);
    }
    return 0;
}
