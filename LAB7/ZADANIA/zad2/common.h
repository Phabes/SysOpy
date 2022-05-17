#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define SHM "M"

#define MAX_OVEN 5
#define MAX_TABLE 5

#define SEM_COUNT 5
char *names[] = {"/SEM1", "/SEM2", "/SEM3", "/SEM4", "/SEM5"};
#define OVEN_ACCESS_SEM 0	// start value: 1. start value tells how many cooks can access the oven at once. Is oven available.
#define OVEN_PLACES_SEM 1	// start value: MAX_OVEN. How many available places are in the oven
#define TABLE_ACCESS_SEM 2   // start value: 1. start value tells how many deliverers can access the table at the same time. Is table available.
#define TABLE_COOK_SEM 3	// start value: MAX_TABLE. How many empty places on the table (for the cooks)
#define TABLE_DELIVERER_SEM 4	// start value: 0. How many pizzas on the table. (for the deliverers)

int running = 1;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

typedef struct{
	int oven[MAX_OVEN];
	int table[MAX_TABLE];
} all_data_shared;

sem_t **get_semaphores(){
    sem_t **semaphores = calloc(sizeof(sem_t), SEM_COUNT);
    for(int i = 0; i < SEM_COUNT; i++){
        sem_t *semaphore = sem_open(names[i], 0);
        if(semaphore == SEM_FAILED){
            printf("error during getting semaphore\n");
            exit(1);
        }
        semaphores[i] = semaphore;
    }
    return semaphores;
}

int get_shared_memory(){
    int shm_id = shm_open(SHM, O_RDWR, 0666);
    if(shm_id == -1){
        printf("error during getting shared memory\n");
        exit(1);
    }
    return shm_id;
}

void increment_semaphore_status(sem_t *semaphore){
    if(sem_post(semaphore) == -1){
        printf("error during incrementing semaphore\n");
        exit(1);
    }
}

void decrement_semaphore_status(sem_t *semaphore){
    if(sem_wait(semaphore) == -1){
        printf("error during incrementing semaphore\n");
        exit(1);
    }
}

int get_number_of_pizzas_in_oven(sem_t *semaphore){
    int val;
    if(sem_getvalue(semaphore, &val) == -1){
        printf("error during getting number of pizzas in oven\n");
        exit(1);
    }
    return MAX_OVEN - val;
}

int get_number_of_pizzas_on_table(sem_t *semaphore){
    int val;
    if(sem_getvalue(semaphore, &val) == -1){
        printf("error during getting number of pizzas on table\n");
        exit(1);
    }
    return val;
}

int get_random_number_of_sec(int lower, int upper){
    return rand() % (upper - lower + 1) + lower;
}

// MAYBE CAN BE BETTER, BECAUSE MILISECONDS ARE FROM ANOTHOR VARIABLE
char *get_current_time(){
    time_t rawtime;
    struct tm *timeinfo;
    struct timeval tve;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
	gettimeofday(&tve, NULL);
    char *buff = malloc(sizeof(char) * 25);
    sprintf(buff, "%02d.%02d.%04d %02d:%02d:%02d:%03ld", timeinfo->tm_mday,
            timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, (tve.tv_usec / 1000) % 1000);
    return buff;
}

#endif //DATA_H