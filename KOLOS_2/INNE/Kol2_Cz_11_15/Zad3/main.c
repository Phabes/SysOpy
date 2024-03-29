#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t var = PTHREAD_COND_INITIALIZER;

int count = 0;

/*
 * Funkcja 'wait_for_counter' powinna czekać, aż zmienna globalna 'count' chroniona
 * przez mutex 'mutex' osiągnie wartość co najmniej 13000, a następnie zwiększyć ją o 13.
 */

void* wait_for_counter(void* arg) {

//...

    printf("Changing value: %d\n", count);
    count += 13;

//...

    pthread_exit(NULL); 
}

int main(void) {
    for (int k = 0; k < 100; ++ k) {
        count = 0;

        pthread_t tid;
        /* Stwórz nowy wątek wykonujący funkcje wait_for_counter.
	*/

        int reps = 100000;
        for (int i = 0; i < reps; ++ i) {
            pthread_mutex_lock(&mutex);
            ++ count;
            pthread_cond_signal(&var);
            pthread_mutex_unlock(&mutex);
        }
        pthread_join(tid, NULL);

        pthread_mutex_lock(&mutex);
        if (count != reps + 13) {
            printf("Wrong value: %d\n", count);
            exit(-1);
        }
        pthread_mutex_unlock(&mutex);
    }
    puts("OK");
}
