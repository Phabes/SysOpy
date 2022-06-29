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

void write_raport_header(FILE *raportFile, char *funcType, char *from, char *to){
    fprintf(raportFile, "%3s\t%15s\t%15s\n", funcType, from, to);
    fprintf(raportFile, "%15s\t%15s\t%15s\n",
            "REAL_TIME",
            "USER_TIME",
            "SYSTEM_TIME");
}

void save_raport(char *funcType, char *from, char *to){
    FILE *raportFile = fopen("pomiar_zad_1.txt", "a");
    if(raportFile == NULL){
        printf("RESULTS SAVE ERROR\n");
        return;
    }
    write_raport_header(raportFile, funcType, from, to);
    save_raport_time(raportFile);
    fclose(raportFile);
}

// ALSO WORKS
// void copy_file_content_lib2(char *from, char *to){
//     FILE *readFile = fopen(from, "r");
//     FILE *writeFile = fopen(to, "a");
//     char sign;
//     int currLineLen = 0;
//     int copyLine = 0;
//     int currTotalLen = 0;
//     while (sign != EOF){
//         sign = getc(readFile);
//         currLineLen++;
//         currTotalLen++;
//         if(isspace(sign) == 0)
//             copyLine = 1;
//         if(sign == '\n' || sign == EOF){
//             //  printf("LENGTH: %d, OK: %d\n", currLineLen, copyLine);
//             if(copyLine == 1){
//                 fseek(readFile, currTotalLen - currLineLen, 0);
//                 if(sign == EOF) currLineLen--;
//                 // char *buffer = malloc(sizeof(char) * currLineLen);
//                 char *buffer = calloc(currLineLen, sizeof(char));
//                 fread(buffer, sizeof(char), currLineLen, readFile);
//                 fwrite(buffer, sizeof(char), currLineLen, writeFile);
//             }
//             currLineLen = 0;
//             copyLine = 0;
//         }
//     }
//     fwrite("\n", sizeof(char), 1, writeFile);
//     fclose(readFile);
//     fclose(writeFile);
// }

void copy_line_lib(FILE *readFile, FILE *writeFile, int currTotalLen, int currLineLen){
    fseek(readFile, currTotalLen - currLineLen, 0);
    char *buffer = calloc(currLineLen, sizeof(char));
    fread(buffer, sizeof(char), currLineLen, readFile);
    fwrite(buffer, sizeof(char), currLineLen, writeFile);
}

void copy_file_content_lib(char *from, char *to){
    FILE *readFile = fopen(from, "r");
    FILE *writeFile = fopen(to, "a");
    char sign;
    int currLineLen = 0;
    int copyLine = 0;
    int currTotalLen = 0;
    while (fread(&sign, sizeof(char), 1, readFile) == 1){
        currLineLen++;
        currTotalLen++;
        if(isspace(sign) == 0)
            copyLine = 1;
        if(sign == '\n'){
            if(copyLine == 1)
                copy_line_lib(readFile, writeFile, currTotalLen, currLineLen);
            currLineLen = 0;
            copyLine = 0;
        }
    }
    if(copyLine == 1){
        copy_line_lib(readFile, writeFile, currTotalLen, currLineLen);
        fwrite("\n", sizeof(char), 1, writeFile);
    }
    fclose(readFile);
    fclose(writeFile);
}

void copy_line_sys(int readFile, int writeFile, int currTotalLen, int currLineLen){
    lseek(readFile, currTotalLen - currLineLen, SEEK_SET);
    char *buffer = calloc(currLineLen, sizeof(char));
    read(readFile, buffer, currLineLen);
    write(writeFile, buffer, currLineLen);
}

void copy_file_content_sys(char *from, char *to){
    int readFile = open(from, O_RDONLY);
    char sign;
    int currLineLen = 0;
    int copyLine = 0;
    int currTotalLen = 0;
    int writeFile = open(to, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
    while(read(readFile, &sign, 1) == 1){
        currLineLen++;
        currTotalLen++;
        if(isspace(sign) == 0)
            copyLine = 1;
        if(sign == '\n'){
            if(copyLine == 1)
                copy_line_sys(readFile, writeFile, currTotalLen, currLineLen);
            currLineLen = 0;
            copyLine = 0;
        }
    }
    if(copyLine == 1){
        copy_line_sys(readFile, writeFile, currTotalLen, currLineLen);
        write(writeFile, "\n", 1);
    }
    close(readFile);
    close(writeFile);
}

int check_file_exist(char *fileName){
    return access(fileName, F_OK);
}

void get_from_file_name(char *arg1){
    printf("Enter file name to copy from\n");
    scanf("%s", arg1);
    while(check_file_exist(arg1) !=0 ){
        printf("Enter proper file name to copy from\n");
        scanf("%s", arg1);
    }
}

void remove_file(char* fileName){
    char str[100] = "rm ";
    system(strcat(str, fileName));
}

int main(int argc, char *argv[]){
    if(argc != 3){
        char arg1[100], arg2[100];
        get_from_file_name(arg1);
        printf("Enter file name to copy to\n");
        scanf("%s", arg2);
        if(check_file_exist(arg2) == 0)
            remove_file(arg2);
        start_timer();
        copy_file_content_sys(arg1, arg2);
        end_timer();
        save_raport("sys", arg1, arg2);
        remove_file(arg2);
        start_timer();
        copy_file_content_lib(arg1, arg2);
        end_timer();
        save_raport("lib", arg1, arg2);
    }
    else{
        if(check_file_exist(argv[1]) != 0){
            char arg1[100];
            get_from_file_name(arg1);
            if(check_file_exist(argv[2]) == 0)
                remove_file(argv[2]);
            start_timer();
            copy_file_content_sys(arg1, argv[2]);
            end_timer();
            save_raport("sys", arg1, argv[2]);
            remove_file(argv[2]);
            start_timer();
            copy_file_content_lib(arg1, argv[2]);
            end_timer();
            save_raport("lib", arg1, argv[2]);
        }
        else{
            if(check_file_exist(argv[2]) == 0)
                remove_file(argv[2]);
            start_timer();
            copy_file_content_sys(argv[1], argv[2]);
            end_timer();
            save_raport("sys", argv[1], argv[2]);
            remove_file(argv[2]);
            start_timer();
            copy_file_content_lib(argv[1], argv[2]);
            end_timer();
            save_raport("lib", argv[1], argv[2]);
        }
    }
    return 0;
}