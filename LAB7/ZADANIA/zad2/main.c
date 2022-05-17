#include "common.h"

sem_t **create_semaphores(){
    unsigned short array[] = {1, MAX_OVEN, 1, MAX_TABLE, 0};
    sem_t **semaphores = calloc(sizeof(sem_t), SEM_COUNT);
    for(int i = 0; i < SEM_COUNT; i++){
        sem_t *semaphore = sem_open(names[i], O_CREAT, 0666, array[i]);
        if(semaphore == SEM_FAILED){
            printf("error during setting semaphore\n");
            exit(1);
        }
        semaphores[i] = semaphore;
    }
    return semaphores;
}

int create_shared_memory(){
    int shm_id = shm_open(SHM, O_CREAT | O_RDWR, 0666);
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

        sem_t **semaphores = create_semaphores();
        int shm_id = create_shared_memory();

        if((ftruncate(shm_id, sizeof(all_data_shared))) == -1){
            printf("error during setting shared memory size\n");
            exit(1);
        }
        all_data_shared *data = mmap(NULL, sizeof(all_data_shared), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
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
        munmap(data, sizeof(all_data_shared));
        shm_unlink(SHM);
        for(int i = 0; i < SEM_COUNT; i++){
            sem_close(semaphores[i]);
            sem_unlink(names[i]);
        }
        printf("PIZZERIA IS CLEANED\n");
    }
    else
        printf("WRONG NUMBER OF PARAMETERS\n");
    return 0;
}