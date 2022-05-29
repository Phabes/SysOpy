#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define REINDEERS 9
#define ELVES 10
#define MAX_ELVES_WAITING 3

int elves_queue[ELVES]; // queue to know who need help
int waiting_elves = 0; // how many elves are waiting
int reindeers_back = 0; // how many reindeers back from holidays
int deliveries_left = 3; // how many deliveries left
int reindeers_busy = 0; // are reindeers busy now
int santa_sleeps = 1; // is santa sleeping now

pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elves_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t delivery_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t solving_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t santa_condition = PTHREAD_COND_INITIALIZER;
pthread_cond_t reindeers_condition = PTHREAD_COND_INITIALIZER;
pthread_cond_t elves_condition = PTHREAD_COND_INITIALIZER;

int get_random_number_of_sec(int lower, int upper){
    return rand() % (upper - lower + 1) + lower;
}

int is_waiting(int elf_id){
    for(int i = 0; i < MAX_ELVES_WAITING; i++){
        if(elves_queue[i] == elf_id)
            return 1;
    }
    return 0;
}

void *santa(void *arg){
    while(1){
        pthread_mutex_lock(&santa_mutex);
        santa_sleeps = 1;
        while(reindeers_back < REINDEERS && waiting_elves < MAX_ELVES_WAITING)
            pthread_cond_wait(&santa_condition, &santa_mutex);
        santa_sleeps = 0;
        pthread_mutex_unlock(&santa_mutex);
        printf("Mikołaj: budzę się\n");
        pthread_mutex_lock(&reindeer_mutex);
        if(reindeers_back == REINDEERS){
            printf("Mikołaj: dostarczam zabawki\n");
            int delivery_time = get_random_number_of_sec(2, 4);
            sleep(delivery_time);
            printf("Mikołaj: dostarczyłem zabawki\n");
            reindeers_back = 0;
            pthread_mutex_lock(&delivery_mutex);
            reindeers_busy = 0;
            pthread_cond_broadcast(&reindeers_condition);
            pthread_mutex_unlock(&delivery_mutex);
            deliveries_left--;
        }
        pthread_mutex_unlock(&reindeer_mutex);
        if(deliveries_left == 0)
            exit(0);
        pthread_mutex_lock(&elves_mutex);
        if(waiting_elves == MAX_ELVES_WAITING){
            char elves_ids[3 * ELVES] = "";
            char index[3];
            for(int i = 0; i < waiting_elves; i++){
                sprintf(index, "%d", elves_queue[i]);
                strcat(elves_ids, index);
                strcat(elves_ids, " ");
            }
            printf("Mikołaj: rozwiązuję problemy elfów %s\n", elves_ids);
            int fixing_time = get_random_number_of_sec(1, 2);
            sleep(fixing_time);
            printf("Mikołaj: rozwiązałem problemy elfów %s\n", elves_ids);
            pthread_mutex_lock(&solving_mutex);
            for(int i = 0; i < waiting_elves; i++)
                elves_queue[i] = -1;
            waiting_elves = 0;
            pthread_cond_broadcast(&elves_condition);
            pthread_mutex_unlock(&solving_mutex);
        }
        pthread_mutex_unlock(&elves_mutex);
        printf("Mikołaj: zasypiam\n");
    }
}

void *reindeer(void *arg){
    int reindeer_id = *(int *)arg;
    while(1){
        int holidays_time = get_random_number_of_sec(5, 10);
        sleep(holidays_time);
        pthread_mutex_lock(&reindeer_mutex);
        reindeers_back++;
        printf("Renifer: czeka %d reniferów na Mikołaja, %d\n", reindeers_back, reindeer_id);
        pthread_mutex_lock(&delivery_mutex);
        reindeers_busy = 1;
        pthread_mutex_unlock(&delivery_mutex);
        if(reindeers_back == REINDEERS){
            printf("Renifer: wybudzam Mikołaja, %d\n", reindeer_id);
            pthread_mutex_lock(&santa_mutex);
            pthread_cond_broadcast(&santa_condition);
            pthread_mutex_unlock(&santa_mutex);
        }
        pthread_mutex_unlock(&reindeer_mutex);
        pthread_mutex_lock(&delivery_mutex);
        while(reindeers_busy == 1)
            pthread_cond_wait(&reindeers_condition, &delivery_mutex);
        pthread_mutex_unlock(&delivery_mutex);
    }
}

void *elf(void *arg){
    int elf_id = *(int *)arg;
    while(1){
        int working_time = get_random_number_of_sec(2, 5);
        sleep(working_time);
        int in_queue = 0;
        do{
            pthread_mutex_lock(&solving_mutex);
            while(waiting_elves == MAX_ELVES_WAITING){
                if(in_queue == 0)
                    printf("Elf: czeka na powrót elfów, %d\n", elf_id);
                pthread_cond_wait(&elves_condition, &solving_mutex);
            }
            pthread_mutex_unlock(&solving_mutex);
            in_queue++;
            pthread_mutex_lock(&elves_mutex);
            if(waiting_elves < MAX_ELVES_WAITING){
                elves_queue[waiting_elves++] = elf_id;
                printf("Elf: czeka %d elfów na Mikołaja, %d\n", waiting_elves, elf_id);
                if(waiting_elves == MAX_ELVES_WAITING){
                    if(santa_sleeps == 1)
                        printf("Elf: wybudzam Mikołaja, %d\n", elf_id);
                    pthread_mutex_lock(&santa_mutex);
                    pthread_cond_broadcast(&santa_condition);
                    pthread_mutex_unlock(&santa_mutex);
                }
                in_queue = -1;
            }
            pthread_mutex_unlock(&elves_mutex);
        } while(in_queue != -1);
        pthread_mutex_lock(&solving_mutex);
        while(is_waiting(elf_id) == 1)
            pthread_cond_wait(&elves_condition, &solving_mutex);
        // pthread_cond_wait(&elves_condition, &solving_mutex); // works too
        pthread_mutex_unlock(&solving_mutex);
    }
}

int main(int argc, char *argv[]){
    for(int i = 0; i < ELVES; i++)
        elves_queue[i] = -1;
    
    pthread_t santa_thread;
    pthread_create(&santa_thread, NULL, &santa, NULL);
    pthread_t reindeers_threads[REINDEERS];
    for(int i = 0; i < REINDEERS; i++){
        int *reindeer_id = malloc(sizeof(int));
        *reindeer_id = i + 1;
        pthread_create(&reindeers_threads[i], NULL, &reindeer, reindeer_id);
    }
    pthread_t elves_threads[ELVES];
    for(int i = 0; i < ELVES; i++){
        int *elf_id = malloc(sizeof(int));
        *elf_id = i + 1;
        pthread_create(&elves_threads[i], NULL, &elf, elf_id);
    }

    pthread_join(santa_thread, NULL);
    for(int i = 0; i < REINDEERS; i++)
        pthread_join(reindeers_threads[i], NULL);
    for(int i = 0; i < ELVES; i++)
        pthread_join(elves_threads[i], NULL);
    return 0;
}