#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>

int main(int argc, char *argv[]){
    if(argc == 5){
        char *fifo_path = argv[1];
        int row = atoi(argv[2]);
        char *file_path = argv[3];
        int n = atoi(argv[4]);
        FILE *fifo_file = fopen(fifo_path, "w");
        FILE *file = fopen(file_path, "r");
        char *buff = malloc(sizeof(char) * n);
        char *data = malloc(sizeof(char) * (n + 3));
        while(fread(buff, sizeof(char), n, file) == n){
            printf("PRODUCER: %s\n", buff);
            data[strlen(data) - 1] = '\n';
            snprintf(data, n + 3, "%d_%s", row, buff);
            fwrite(data, sizeof(char), n + 3, fifo_file);
            sleep(1);
        }
        fclose(fifo_file);
        fclose(file);
    }
    else
        printf("WRONG NUMBER OF PARAMS\n");
    return 0;
}