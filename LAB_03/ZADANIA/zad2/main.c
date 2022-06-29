#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include<sys/wait.h>
#include <time.h>
#include <sys/times.h>

clock_t start_time, end_time;
struct tms start_CPU, end_CPU;

void start_timer(){
    start_time = times(&start_CPU);
}

void end_timer(){
    end_time = times(&end_CPU);
}

void save_raport_time(FILE *raport_file){
    int clk_tics = sysconf(_SC_CLK_TCK);
    double real_time = (double)(end_time - start_time) / clk_tics;
    double user_time = (double)(end_CPU.tms_utime - start_CPU.tms_utime) / clk_tics;
    double system_time = (double)(end_CPU.tms_stime - start_CPU.tms_stime) / clk_tics;
    fprintf(raport_file, "%15f\t%15f\t%15f\n",
            real_time,
            user_time,
            system_time);
}

void write_raport_header(FILE *raport_file, int blocks, int n, double sum){
    fprintf(raport_file, "\t%10d\t%10d\t%12f\n", blocks, n, sum);
    fprintf(raport_file, "%15s\t%15s\t%15s\n",
            "REAL_TIME",
            "USER_TIME",
            "SYSTEM_TIME");
}

void save_raport(int blocks, int n, double sum){
    FILE *raport_file = fopen("pomiar_zad_2.txt", "a");
    if(raport_file == NULL){
        printf("RESULTS SAVE ERROR\n");
        return;
    }
    write_raport_header(raport_file, blocks, n, sum);
    save_raport_time(raport_file);
    fclose(raport_file);
}

int is_number(char *number){
    int length = strlen(number);
    for(int i = 0; i < length; i++){
        if(isdigit(number[i]) == 0)
            return 0;
    }
    return 1;
}

int number_length(int i){
    int length = 0;
    while(i!=0){
        length++;
        i /= 10;
    }
    return length;
}

double calculate_block_range(double start, double end, int blocks){
    return (end - start) / blocks;
}

double f(double x){
    return 4 / (x * x + 1);
}

void write_to_file(int i, double sum){
    int i_length = number_length(i);
    // +5 means: 1 - w, 4 - .txt
    int length = i_length + 5;
    char *file_name = calloc(length, sizeof(char));
    snprintf(file_name, length + 1, "w%d.txt", i);
    FILE *write_file = fopen(file_name, "w");
    if(write_file == NULL){
        printf("COULDNT CREATE FILE\n");
        return;
    }
    fprintf(write_file, "%f", sum);
    free(file_name);
    file_name = NULL;
    fclose(write_file);
}

double calculate_sum_files(int n){
    double sum = 0;
    for(int i = 1; i <= n; i++){
        int i_length = number_length(i);
        // +5 means: 1 - w, 4 - .txt
        int length = i_length + 5;
        char *file_name = calloc(length, sizeof(char));
        snprintf(file_name, length + 2, "w%d.txt", i);
        FILE *read_file = fopen(file_name, "r");
        if(read_file == NULL)
            printf("COULDNT OPEN FILE\n");
        else{
            fseek(read_file, 0L, SEEK_END);
            int size = ftell(read_file);
            rewind(read_file);
            char *number = calloc(size, sizeof(char));
            fread(number, sizeof(char), size, read_file);
            sum += atof(number);
            free(number);
            number = NULL;
        }
        free(file_name);
        file_name = NULL;
        fclose(read_file);
    }
    return sum;
}

double calculate_process_part(double range, int number_of_blocks, int used, int index, double start){
    double sum = 0;
    for(int i = 1; i <= number_of_blocks; i++){
        // double x = start + (used + i) * range; // range end point
        double x = ((start + (used + i - 1)) + (start + (used + i))) / 2 * range; // range middle point
        sum += f(x) * range;
    }
    write_to_file(index, sum);
    return sum;
}

int main(int argc, char *argv[]){
    double start = 0;
    double end = 1;
    if(argc == 3){
        if(is_number(argv[1]) == 1 && is_number(argv[2]) == 1){
            start_timer();
            int blocks = atoi(argv[1]);
            int n = atoi(argv[2]);
            int each = blocks / n;
            int additional = blocks % n;
            double range = calculate_block_range(start, end, blocks);
            int used = 0;
            for(int i = 1; i <= n; i++){
                int number = each;
                if(i <= additional)
                    number += 1;
                if(fork() == 0){
                    calculate_process_part(range, number, used, i, start);
                    exit(0);
                }
                used += number;
            }
            int status;
            while(wait(&status) >= 0);
            double sum = calculate_sum_files(n);
            printf("SUM: %f\n", sum);
            end_timer();
            save_raport(blocks, n, sum);
        }
        else
            printf("WRONG PARAMS\n");
    }
    else
        printf("WRONG NUMBER OF PARAMS\n");
    return 0;
}