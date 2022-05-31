#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CONSUMER_PROGRAM "./consumer"
#define PRODUCER_PROGRAM "./producer"
#define LINE_SIZE 256

void create_empty_file(char* filename, int rows){
    FILE* file = fopen(filename, "w");
    char* line = calloc(LINE_SIZE, sizeof(char));
    for (int i = 0; i < LINE_SIZE - 1; i++)
        line[i] = ' ';
    line[LINE_SIZE - 1] = '\n';
    for (int i = 0; i < rows - 1; i++)
        fprintf(file, "%s", line);
    line[LINE_SIZE - 1] = '\0';
    fprintf(file, "%s", line);
    fclose(file);
    free(line);
}

void many_producers_one_consumer(char *fifo_path){
    int consumers = 1;
    int producers = 5;
    char *consumer_file = "consumer_files/many_producers_one_consumer.txt";
    char *producers_files[] = {"producer_files/0.txt", "producer_files/2.txt", "producer_files/9.txt", "producer_files/P.txt", "producer_files/Y.txt"};
    char *consumer_n = "10";
    char *producer_n = "7"; // 3 less than consumer_n

    for(int i = 0; i < consumers; i++){
        if(fork() == 0){
            char *args[] = {CONSUMER_PROGRAM, fifo_path, consumer_file, consumer_n, NULL};
            if(execvp(args[0], args) == -1)
                printf("ERROR WHILE CONSUMER EXECL\n");
        }
    }
    create_empty_file(consumer_file, producers);
    for(int i = 0; i < producers; i++){
        char row[2];
        sprintf(row, "%d", i);
        if(fork() == 0){
            char *args[] = {PRODUCER_PROGRAM, fifo_path, row, producers_files[i], producer_n, NULL};
            if(execvp(args[0], args) == -1)
                printf("ERROR WHILE PRODUCER EXECL\n");
        }
    }
    for(int i = 0; i < consumers + producers; i++)
        wait(NULL);
}

void one_producer_many_consumers(char *fifo_path){
    int consumers = 5;
    int producers = 1;
    char *consumer_file = "consumer_files/one_producer_many_consumers.txt";
    char *producers_file = "producer_files/9.txt";
    char *consumer_n = "5";
    char *producer_n = "2"; // 3 less than consumer_n

    for(int i = 0; i < consumers; i++){
        if(fork() == 0){
            char *args[] = {CONSUMER_PROGRAM, fifo_path, consumer_file, consumer_n, NULL};
            if(execvp(args[0], args) == -1)
                printf("ERROR WHILE CONSUMER EXECL\n");
        }
    }
    create_empty_file(consumer_file, producers);
    for(int i = 0; i < producers; i++){
        char row[2];
        sprintf(row, "%d", i);
        if(fork() == 0){
            char *args[] = {PRODUCER_PROGRAM, fifo_path, row, producers_file, producer_n, NULL};
            if(execvp(args[0], args) == -1)
                printf("ERROR WHILE PRODUCER EXECL\n");
        }
    }
    for(int i = 0; i < consumers + producers; i++)
        wait(NULL);
}

void many_producers_many_consumers(char *fifo_path){
    int consumers = 5;
    int producers = 5;
    char *consumer_file = "consumer_files/many_producers_many_consumers.txt";
    char *producers_files[] = {"producer_files/0.txt", "producer_files/2.txt", "producer_files/9.txt", "producer_files/P.txt", "producer_files/Y.txt"};
    char *consumer_n = "8";
    char *producer_n = "5"; // 3 less than consumer_n

    for(int i = 0; i < consumers; i++){
        if(fork() == 0){
            char *args[] = {CONSUMER_PROGRAM, fifo_path, consumer_file, consumer_n, NULL};
            if(execvp(args[0], args) == -1)
                printf("ERROR WHILE CONSUMER EXECL\n");
        }
    }
    create_empty_file(consumer_file, producers);
    for(int i = 0; i < producers; i++){
        char row[2];
        sprintf(row, "%d", i);
        if(fork() == 0){
            char *args[] = {PRODUCER_PROGRAM, fifo_path, row, producers_files[i], producer_n, NULL};
            if(execvp(args[0], args) == -1)
                printf("ERROR WHILE PRODUCER EXECL\n");
        }
    }
    for(int i = 0; i < consumers + producers; i++)
        wait(NULL);
}

int main(int argc, char *argv[]){
    char *fifo_path = "FIFO";
    mkfifo(fifo_path, 0666);
    many_producers_one_consumer(fifo_path);
    one_producer_many_consumers(fifo_path);
    many_producers_many_consumers(fifo_path);
    return 0;
}