#include "common.h"

int create_semaphores(){
    key_t key = ftok(HOME, SEM);
    int sem_id= semget(key, SEM_COUNT, BASE_FLAGS);
    if(sem_id == -1){
        printf("error during creating group of semaphores\n");
        exit(1);
    }
    union semun u;
    // I SP
    unsigned short array[] = {1, MAX_OVEN, 1, MAX_TABLE, 0};
    u.array = array;
    if(semctl(sem_id, SEM_NUM, SETALL, u) == -1){
        printf("error during setting semaphores\n");
        exit(1);
    }
    // II SP
    // u.val = 1;
    // set_semaphore_status(sem_id, OVEN_ACCESS_SEM, u);
    // set_semaphore_status(sem_id, TABLE_ACCESS_SEM, u);
    // u.val = 0;
    // set_semaphore_status(sem_id, TABLE_DELIVERER_SEM, u);
    // u.val = MAX_OVEN;
    // set_semaphore_status(sem_id, OVEN_PLACES_SEM, u);
    // u.val = MAX_TABLE;
    // set_semaphore_status(sem_id, TABLE_COOK_SEM, u);
    return sem_id;
}

int create_shared_memory(){
    key_t key = ftok(HOME, SHM);
    int shm_id = shmget(key, sizeof(all_data_shared), BASE_FLAGS);
    if(shm_id == -1){
        printf("error during creating shared memory\n");
        exit(1);
    }
    return shm_id;
}

void init_data(all_data_shared *data){
    for(int i = 0; i < MAX_OVEN; i++)
        data -> oven[i] = -1;
    for(int i = 0; i < MAX_TABLE; i++)
        data -> table[i] = -1;
}

void create_cooks(int cooks, pid_t *workers_pids){
    char index[2];
    for(int i = 0; i < cooks; i++){
        pid_t cook_pid;
        cook_pid = fork();
        if(cook_pid == 0){
            sprintf(index, "%d", i);
            execl("./cook", "./cook", index, NULL);
        }
        else
            workers_pids[i] = cook_pid;
    }
}

void create_deliverers(int deliverers, pid_t *workers_pids, int start_index){
    char index[2];
    for(int i = 0; i < deliverers; i++){
        pid_t deliverer_pid;
        deliverer_pid = fork();
        if(deliverer_pid == 0){
            sprintf(index, "%d", i);
            execl("./deliverer", "./deliverer", index, NULL);
        }
        else
            workers_pids[start_index + i] = deliverer_pid;
    }
}

void handler(int sig){
    running = 0;
    printf("\nRESTAURANT IS BEING CLOSED: %d\n", running);
}

int main(int argc, char *argv[]){
    if(argc == 3){
        int cooks = atoi(argv[1]);
        int deliverers = atoi(argv[2]);
        pid_t workers_pids[cooks + deliverers];

        int sem_id = create_semaphores();
        int shm_id = create_shared_memory();

        all_data_shared *data = shmat(shm_id, NULL, 0);
        init_data(data);
        
        create_cooks(cooks, workers_pids);
        create_deliverers(deliverers, workers_pids, cooks);

        signal(SIGINT, handler);

        while(running == 1)
            usleep(200);

        for(int i = 0; i < cooks + deliverers; i++){
            kill(workers_pids[i], SIGINT);
            wait(NULL);
        }

        printf("ALL HAVE DONE THEIR JOB\n");
        shmdt(data);
        shmctl(shm_id, IPC_RMID, 0);
        semctl(sem_id, SEM_NUM, IPC_RMID);
        printf("PIZZERIA IS CLEANED\n");
    }
    else
        printf("WRONG NUMBER OF PARAMETERS\n");
    return 0;
}