#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/wait.h>

#define FILE_NAME "common.txt"
#define SEM_NAME "/kol_sem"

int main(int argc, char **args) {

    if (argc != 4) {
        printf("Not a suitable number of program parameters\n");
        return (1);
    }

    /**************************************************
    Stworz semafor systemu V
    Ustaw jego wartosc na 1
    ***************************************************/
    key_t key = ftok(SEM_NAME, 'q');
    int sem_id = semget(key, 1, IPC_CREAT | 0666);
    semctl(sem_id, 0, SETVAL, 1);

    int fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    int parentLoopCounter = atoi(args[1]);
    int childLoopCounter = atoi(args[2]);

    char buf[50];
    pid_t childPid;
    int max_sleep_time = atoi(args[3]);


    if ((childPid = fork())) {
        int status = 0;
        srand((unsigned) time(0));

        while (parentLoopCounter--) {
            int s = rand() % max_sleep_time + 1;
            sleep(s);

            /*****************************************
            sekcja krytyczna zabezpiecz dostep semaforem
            **********************************************/
            struct sembuf buffer;
            buffer.sem_num = 0;
            buffer.sem_op = -1;
            buffer.sem_flg = 0;
            semop(sem_id, &buffer, 1);

            sprintf(buf, "Wpis rodzica. Petla %d. Spalem %d\n", parentLoopCounter, s);
            write(fd, buf, strlen(buf));
            write(1, buf, strlen(buf));

            /*********************************
            Koniec sekcji krytycznej
            **********************************/
            buffer.sem_op = 1;
            semop(sem_id, &buffer, 1);

        }
        waitpid(childPid, &status, 0);
    } else {
        srand((unsigned) time(0));
        while (childLoopCounter--) {

            int s = rand() % max_sleep_time + 1;
            sleep(s);

            /*****************************************
            sekcja krytyczna zabezpiecz dostep semaforem
            **********************************************/
            struct sembuf buffer;
            buffer.sem_num = 0;
            buffer.sem_op = -1;
            buffer.sem_flg = 0;
            semop(sem_id, &buffer, 1);


            sprintf(buf, "Wpis dziecka. Petla %d. Spalem %d\n", childLoopCounter, s);
            write(fd, buf, strlen(buf));
            write(1, buf, strlen(buf));

            /*********************************
            Koniec sekcji krytycznej
            **********************************/
            buffer.sem_op = 1;
            semop(sem_id, &buffer, 1);

        }
        _exit(0);
    }
    /*****************************
    posprzataj semafor
    ******************************/
    semctl(sem_id, 0, IPC_RMID);
    close(fd);
    return 0;
}
     
        
