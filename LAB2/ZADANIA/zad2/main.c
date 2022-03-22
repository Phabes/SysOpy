#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/times.h>

clock_t startTime, endTime;
struct tms startCPU, endCPU;

void start_timer(){
    startTime = times(&startCPU);
}

void end_timer(){
    endTime = times(&endCPU);
}

void save_raport_time(FILE *raportFile){
    int clkTics = sysconf(_SC_CLK_TCK);
    double realTime = (double)(endTime - startTime) / clkTics;
    double userTime = (double)(endCPU.tms_utime - startCPU.tms_utime) / clkTics;
    double systemTime = (double)(endCPU.tms_stime - startCPU.tms_stime) / clkTics;
    fprintf(raportFile, "%15f\t%15f\t%15f\n",
            realTime,
            userTime,
            systemTime);
}

void write_raport_header(FILE *raportFile, char *funcType, char c, char *fileName, int *charCounts, int *lineCounts){
    fprintf(raportFile, "%3s\t%c\t%15s\t%10d\t%10d\n", funcType, c, fileName, *charCounts, *lineCounts);
    fprintf(raportFile, "%15s\t%15s\t%15s\n",
            "REAL_TIME",
            "USER_TIME",
            "SYSTEM_TIME");
}

void save_raport(char *funcType, char c, char *fileName, int *charCounts, int *lineCounts){
    FILE *raportFile = fopen("pomiar_zad_2.txt", "a");
    if(raportFile == NULL){
        printf("RESULTS SAVE ERROR\n");
        return;
    }
    write_raport_header(raportFile, funcType, c, fileName, charCounts, lineCounts);
    save_raport_time(raportFile);
    fclose(raportFile);
}

void count_lines_chars_lib(char c, char *fileName, int *charCounts, int *lineCounts){
    FILE *readFile = fopen(fileName, "r");
    char sign;
    *charCounts = 0;
    *lineCounts = 0;
    int lineContains = 0;
    while (fread(&sign, sizeof(char), 1, readFile) == 1){
        if(c == sign){
            (*charCounts)++;
            if(lineContains == 0){
                lineContains = 1;
                (*lineCounts)++;
            }
        }
        if(sign == '\n')
            lineContains = 0;
    }
    printf("LINES: %d, CHARS: %d\n", *lineCounts, *charCounts);
    fclose(readFile);
}

void count_lines_chars_sys(char c, char *fileName, int *charCounts, int *lineCounts){
    int readFile = open(fileName, O_RDONLY);
    char sign;
    *charCounts = 0;
    *lineCounts = 0;
    int lineContains = 0;
    while(read(readFile, &sign, 1) == 1){
        if(c == sign){
            (*charCounts)++;
            if(lineContains == 0){
                lineContains = 1;
                (*lineCounts)++;
            }
        }
        if(sign == '\n')
            lineContains = 0;
    }
    printf("LINES: %d, CHARS: %d\n", *lineCounts, *charCounts);
    close(readFile);
}

int check_file_exist(char *fileName){
    return access(fileName, F_OK);
}

int main(int argc, char *argv[]){
    if(argc == 3){
        char *fileName = argv[2];
        if(check_file_exist(fileName) == 0){
            if(strlen(argv[1]) == 1){
                char c = argv[1][0];
                int charCounts = 0;
                int lineCounts = 0;
                start_timer();
                count_lines_chars_sys(c, fileName, &charCounts, &lineCounts);
                end_timer();
                save_raport("sys", c, fileName, &charCounts, &lineCounts);
                start_timer();
                count_lines_chars_lib(c, fileName, &charCounts, &lineCounts);
                end_timer();
                save_raport("lib", c, fileName, &charCounts, &lineCounts);
            }
            else{
                printf("PASS SINGLE CHARACTER\n");
            }
        }
        else{
            printf("FILE NOT FOUND\n");
        }
    }
    return 0;
}