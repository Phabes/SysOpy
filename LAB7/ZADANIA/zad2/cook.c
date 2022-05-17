#include "common.h"

int get_empty_place_in_oven(all_data_shared *data){
    for(int i = 0; i < MAX_OVEN; i++){
        if(data -> oven[i] == -1)
            return i;
    }
    return -1;
}

int get_empty_place_on_table(all_data_shared *data){
    for(int i = 0; i < MAX_TABLE; i++){
        if(data -> table[i] == -1)
            return i;
    }
    return -1;
}

int put_pizza_into_oven(sem_t **semaphores, all_data_shared *data, int pizza_type){
    decrement_semaphore_status(semaphores[OVEN_PLACES_SEM]);
    decrement_semaphore_status(semaphores[OVEN_ACCESS_SEM]);
    int position_in_oven = get_empty_place_in_oven(data);
    data -> oven[position_in_oven] = pizza_type;
    increment_semaphore_status(semaphores[OVEN_ACCESS_SEM]);
    return position_in_oven;
}

int take_pizza_from_oven(sem_t **semaphores, all_data_shared *data, int position_in_oven){
    decrement_semaphore_status(semaphores[OVEN_ACCESS_SEM]);
    int pizza_type = data -> oven[position_in_oven];
    data -> oven[position_in_oven] = -1;
    increment_semaphore_status(semaphores[OVEN_ACCESS_SEM]);
    increment_semaphore_status(semaphores[OVEN_PLACES_SEM]);
    return pizza_type;
}

void put_pizza_on_table(sem_t **semaphores, all_data_shared *data, int pizza_type){
    decrement_semaphore_status(semaphores[TABLE_COOK_SEM]);
    decrement_semaphore_status(semaphores[TABLE_ACCESS_SEM]);
    int position_on_table = get_empty_place_on_table(data);
    data -> table[position_on_table] = pizza_type;
    increment_semaphore_status(semaphores[TABLE_ACCESS_SEM]);
    increment_semaphore_status(semaphores[TABLE_DELIVERER_SEM]);
}

int main(int argc, char *argv[]){
    if(argc == 2){
        int cook_id = atoi(argv[1]);
        srand(time(0) + cook_id);

        sem_t **semaphores = get_semaphores();
        int shm_id = get_shared_memory();

        all_data_shared *data = mmap(NULL, sizeof(all_data_shared), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);

        while(running == 1){
            int pizza_type = rand() % 10;
            int preparing_time = get_random_number_of_sec(1, 2);
            sleep(preparing_time);
            printf("(%d %s) Przygowuję pizze: %d\n", getpid(), get_current_time(), pizza_type);
            // printf("(%d %s) Przygowuję pizze: %d\n", cook_id, get_current_time(), pizza_type);
            int position_in_oven = put_pizza_into_oven(semaphores, data, pizza_type);
            printf("(%d %s) Dodałem pizze: %d. Liczba pizz w piecu: %d\n", getpid(), get_current_time(), pizza_type, get_number_of_pizzas_in_oven(semaphores[OVEN_PLACES_SEM]));
            // printf("(%d %s) Dodałem pizze: %d. Liczba pizz w piecu: %d\n", cook_id, get_current_time(), pizza_type, get_number_of_pizzas_in_oven(semaphores[OVEN_PLACES_SEM]));
            int baking_time = get_random_number_of_sec(4, 5);
            sleep(baking_time);
            int taken_pizza_type = take_pizza_from_oven(semaphores, data, position_in_oven);
            int pizzas_in_oven = get_number_of_pizzas_in_oven(semaphores[OVEN_PLACES_SEM]);
            if(pizzas_in_oven == MAX_OVEN)
                pizzas_in_oven--;
            printf("(%d %s) Wyjmuję pizze: %d. Liczba pizz w piecu: %d\n", getpid(), get_current_time(), taken_pizza_type, pizzas_in_oven);
            // printf("(%d %s) Wyjmuję pizze: %d. Liczba pizz w piecu: %d\n", cook_id, get_current_time(), taken_pizza_type, pizzas_in_oven);
            put_pizza_on_table(semaphores, data, taken_pizza_type);
            int pizzas_on_table = get_number_of_pizzas_on_table(semaphores[TABLE_DELIVERER_SEM]);
            if(pizzas_on_table == 0)
                pizzas_on_table++;
            printf("(%d %s) Kładę pizze: %d. Liczba pizz na stole: %d\n", getpid(), get_current_time(), taken_pizza_type, pizzas_on_table);
            // printf("(%d %s) Kładę pizze: %d. Liczba pizz na stole: %d\n", cook_id, get_current_time(), taken_pizza_type, pizzas_on_table);
        }
    }
    else
        printf("COOK WRONG NUMBER OF PARAMETERS\n");
    return 0;
}