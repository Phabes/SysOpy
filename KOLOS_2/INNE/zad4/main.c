﻿#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX 20
#define KEY "./path"

int main() {
    int shmid, i;
    int *buf;
    //Wygeneruj klucz dla kolejki na podstawie KEY i znaku 'a'
    key_t key = ftok(KEY, 'a');

    //Utwórz segment pamięci współdzielonej shmid o prawach dostępu 600, rozmiarze MAX
    //jeśli segment już istnieje, zwróć błąd, jeśli utworzenie pamięci się nie powiedzie zwróć błąd i wartość 1
    shmid = shmget(key, MAX, IPC_CREAT | IPC_EXCL | 0600);
    if(shmid == -1){
        printf("SHMGET ERROR\n");
        return 1;
    }

    //Przyłącz segment pamięci współdzielonej do procesu do buf, obsłuż błędy i zwróć 1
    buf = shmat(shmid, NULL, 0);
    if (buf == (void *) -1) {
        printf("SHMAT ERROR\n");
        return 1;
    }

    for (i = 0; i < MAX; i++) {
        buf[i] = i * i;
        printf("Wartość: %d \n", buf[i]);
    }
    printf("Memory written\n");
    //odłącz i usuń segment pamięci współdzielonej
    shmdt(buf);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
