#include "common.h"

int get_busy_place_on_table(all_data_shared *data){
    for(int i = 0; i < MAX_TABLE; i++){
        if(data -> table[i] != -1)
            return i;
    }
    return -1;
}

int take_pizza_from_table(int sem_id, all_data_shared *data){
    change_semaphore_status(sem_id, TABLE_DELIVERER_SEM, -1);
    change_semaphore_status(sem_id, TABLE_ACCESS_SEM, -1);
    int position_on_table = get_busy_place_on_table(data);
    int taken_pizza_type = data -> table[position_on_table];
    data -> table[position_on_table] = -1;
    change_semaphore_status(sem_id, TABLE_ACCESS_SEM, 1);
    change_semaphore_status(sem_id, TABLE_COOK_SEM, 1);
    return taken_pizza_type;
}

int main(int argc, char *argv[]){
    if(argc == 2){
        int deliverer_id = atoi(argv[1]);
        srand(time(0) + deliverer_id);

        int sem_id = get_semaphore();
        int shm_id = get_shared_memory();

        all_data_shared *data = shmat(shm_id, NULL, 0);

        while(running == 1){
            int taken_pizza_type = take_pizza_from_table(sem_id, data);
            printf("(%d %s) Pobieram pizze: %d. Liczba pizz na stole: %d\n", getpid(), get_current_time(), taken_pizza_type, get_number_of_pizzas_on_table(sem_id));
            // printf("(%d %s) Pobieram pizze: %d. Liczba pizz na stole: %d\n", deliverer_id, get_current_time(), taken_pizza_type, get_number_of_pizzas_on_table(sem_id));
            int delivery_time = get_random_number_of_sec(4, 5);
            sleep(delivery_time);
            printf("(%d %s) Dostarczam pizze: %d\n", getpid(), get_current_time(), taken_pizza_type);
            // printf("(%d %s) Dostarczam pizze: %d\n", deliverer_id, get_current_time(), taken_pizza_type);
            int back_time = get_random_number_of_sec(4, 5);
            sleep(back_time);
        }
    }
    else
        printf("COOK WRONG NUMBER OF PARAMETERS\n");
    return 0;
}