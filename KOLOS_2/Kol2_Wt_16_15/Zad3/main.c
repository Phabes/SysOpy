#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <unistd.h>

#define THREADS_NR 9

void *hello(void *arg) {
//Napisz funkcję wypisującą komunikat typu: Hello  (wartość_TID): (numer_TID wątku)
// i zwracającą status o wartości równej agrumentowi przesłanemu do funkcji.
    int x = *(int *)arg;
    printf("Hello  (%d): (%d)\n", (int) pthread_self(), x);
    pthread_exit((void *) x);
//koniec
}

int main(int argc, char* argv[]) {
    pthread_t hello_threads[THREADS_NR];
    int *count = malloc(sizeof(int));
    int i;

//Utwórz THREADS_NR wątków wywołujących funkcję hello z argumentami kolejno inkrementowanymi od wartości 1
//wraz z obsługą błędów w przypadku tworzenia wątków
    for(i = 0; i < THREADS_NR; i++){
        *count = i + 1;
        int create = pthread_create(&hello_threads[i], NULL, hello, (void *) count);
        if(create != 0){
            printf("PTHREAD_CREATE ERROR\n");
            exit(0);
        }

//koniec
        usleep(1000);
    }
    int *hello_results[THREADS_NR];
    for (i=0; i<THREADS_NR;i++) {
     pthread_join(hello_threads[i], (void **) &hello_results[i]);
     printf("Thread %d TID:  %d returned value %d.\n", i+1, (int) hello_threads[i], hello_results[i]);
    }
    return 0;
}
