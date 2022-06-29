#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/file.h>

#define LINE_SIZE 256

void save_to_file(FILE *file, int index, char *data){
    rewind(file);
    flock(fileno(file), LOCK_EX);
    fseek(file, index * LINE_SIZE , SEEK_SET);
    fprintf(file, data, strlen(data) + 1);
    fflush(file);
    flock(fileno(file), LOCK_UN);
}

int main(int argc, char *argv[]){
    if(argc == 4){
        char *fifo_path = argv[1];
        char *file_path = argv[2];
        int n = atoi(argv[3]);
        FILE *fifo_file = fopen(fifo_path, "r");
        FILE *file = fopen(file_path, "rw+");
        char *buff = malloc(sizeof(char) * n);
        while(fread(buff, sizeof(char), n, fifo_file) == n){
            printf("CONSUMER: %s\n", buff);
            char *element = strtok(buff, "_");
            int index = atoi(element);
            element = strtok(NULL, "\n");
            save_to_file(file, index, element);
        }
        fclose(fifo_file);
        fclose(file);
    }
    else
        printf("WRONG NUMBER OF PARAMETERS\n");
    return 0;
}