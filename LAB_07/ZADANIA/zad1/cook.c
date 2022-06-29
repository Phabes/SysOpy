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

int put_pizza_into_oven(int sem_id, all_data_shared *data, int pizza_type){
    change_semaphore_status(sem_id, OVEN_PLACES_SEM, -1);
    change_semaphore_status(sem_id, OVEN_ACCESS_SEM, -1);
    int position_in_oven = get_empty_place_in_oven(data);
    data -> oven[position_in_oven] = pizza_type;
    change_semaphore_status(sem_id, OVEN_ACCESS_SEM, 1);
    return position_in_oven;
}

int take_pizza_from_oven(int sem_id, all_data_shared *data, int position_in_oven){
    change_semaphore_status(sem_id, OVEN_ACCESS_SEM, -1);
    int pizza_type = data -> oven[position_in_oven];
    data -> oven[position_in_oven] = -1;
    change_semaphore_status(sem_id, OVEN_ACCESS_SEM, 1);
    change_semaphore_status(sem_id, OVEN_PLACES_SEM, 1);
    return pizza_type;
}

void put_pizza_on_table(int sem_id, all_data_shared *data, int pizza_type){
    change_semaphore_status(sem_id, TABLE_COOK_SEM, -1);
    change_semaphore_status(sem_id, TABLE_ACCESS_SEM, -1);
    int position_on_table = get_empty_place_on_table(data);
    data -> table[position_on_table] = pizza_type;
    change_semaphore_status(sem_id, TABLE_ACCESS_SEM, 1);
    change_semaphore_status(sem_id, TABLE_DELIVERER_SEM, 1);
}

int main(int argc, char *argv[]){
    if(argc == 2){
        int cook_id = atoi(argv[1]);
        srand(time(0) + cook_id);

        int sem_id = get_semaphore();
        int shm_id = get_shared_memory();

        all_data_shared *data = shmat(shm_id, NULL, 0);

        while(running == 1){
            int pizza_type = rand() % 10;
            int preparing_time = get_random_number_of_sec(1, 2);
            sleep(preparing_time);
            printf("(%d %s) Przygowuję pizze: %d\n", getpid(), get_current_time(), pizza_type);
            // printf("(%d %s) Przygowuję pizze: %d\n", cook_id, get_current_time(), pizza_type);
            int position_in_oven = put_pizza_into_oven(sem_id, data, pizza_type);
            printf("(%d %s) Dodałem pizze: %d. Liczba pizz w piecu: %d\n", getpid(), get_current_time(), pizza_type, get_number_of_pizzas_in_oven(sem_id));
            // printf("(%d %s) Dodałem pizze: %d. Liczba pizz w piecu: %d\n", cook_id, get_current_time(), pizza_type, get_number_of_pizzas_in_oven(sem_id));
            int baking_time = get_random_number_of_sec(4, 5);
            sleep(baking_time);
            int taken_pizza_type = take_pizza_from_oven(sem_id, data, position_in_oven);
            int pizzas_in_oven = get_number_of_pizzas_in_oven(sem_id);
            if(pizzas_in_oven == MAX_OVEN)
                pizzas_in_oven--;
            printf("(%d %s) Wyjmuję pizze: %d. Liczba pizz w piecu: %d\n", getpid(), get_current_time(), taken_pizza_type, pizzas_in_oven);
            // printf("(%d %s) Wyjmuję pizze: %d. Liczba pizz w piecu: %d\n", cook_id, get_current_time(), taken_pizza_type, pizzas_in_oven);
            put_pizza_on_table(sem_id, data, taken_pizza_type);
            int pizzas_on_table = get_number_of_pizzas_on_table(sem_id);
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