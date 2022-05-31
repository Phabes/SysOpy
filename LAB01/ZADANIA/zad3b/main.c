#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/resource.h>
#include <time.h>
#ifdef dynamic 
#include <dlfcn.h>
#endif

clock_t st_time, en_time;
struct tms st_cpu, en_cpu;

int parseCommand(char *command){
    if(strcmp(command, "create_table") == 0){
        return 1;
    }
    if(strcmp(command, "wc_files") == 0){
        return 2;
    }
    if(strcmp(command, "remove_block") == 0){
        return 3;
    }
    if(strcmp(command, "delete_table") == 0){
        return 4;
    }
    return -1;
}

int detectNextCommand(char *argv[], int argc, int i){
    for(int j=i;j<argc;j++){
        if(parseCommand(argv[j])!=-1)
            return j;
    }
    return argc;
}

int checkIfFileExists(char *fileName){
    FILE *fPointer = fopen(fileName, "r");
    if(fPointer==NULL){
        printf("FILE %s DOESNT EXIST", fileName);
        return -1;
    }
    fclose(fPointer);
    return 1;
}

int createWcCommand(char *argv[], int start, int end, char *wc){
    int ok = 0;
    for (int i=start+1;i<end;i++){
        if(checkIfFileExists(argv[i])){
            strcat(wc, " ");
            strcat(wc, argv[i]);
            ok = 1;
        }
    }
    strcat(wc, " >> ");
    return ok;
}

void startTimer()
{
    st_time = times(&st_cpu);
}

void endTimer()
{
    en_time = times(&en_cpu);
}

void saveTimeRaport(char *name, char *args){
    FILE *fPointer = fopen("raport3b.txt", "a");
    if(fPointer==NULL){
        printf("FILE not found\n");
        return;
    }
    int clk_tics = sysconf(_SC_CLK_TCK);
    double real_time = (double)(en_time - st_time) / clk_tics;
    double user_time = (double)(en_cpu.tms_utime - st_cpu.tms_utime) / clk_tics;
    double system_time = (double)(en_cpu.tms_stime - st_cpu.tms_stime) / clk_tics;
    fprintf(fPointer, "%20s:%20s%15f\t%15f\t%15f\n",
            name,
            args,
            real_time,
            user_time,
            system_time);
    fclose(fPointer);
}

void writeHeader(){
    FILE *fPointer = fopen("raport3b.txt", "a");
    if(fPointer==NULL){
        printf("FILE not found\n");
        return;
    }
    fprintf(fPointer, "%20s:%20s%15s\t%15s\t%15s\n",
            "COMMAND",
            "ARGS",
            "REAL_TIME",
            "USER_TIME",
            "SYSTEM_TIME");
    fclose(fPointer);
}

void printAll(char **tab, int n){
    for(int i=0;i<n;i++){
        printf("com: %p\n", tab[i]);
    }
}

int main(int argc, char *argv[])
{
    #ifdef dynamic
        void* handle = dlopen("./lib_wc.so", RTLD_NOW);
        if(!handle) {
            fprintf(stderr, "dlopen() %s\n", dlerror());
            exit(1);
        }
        char **(*createTable)(int) = dlsym(handle, "createTable");
        // void (*wcFiles)(char *) = dlsym(handle, "wcFiles");
        // void (*clearFile)(char *) = dlsym(handle, "clearFile");
        // int (*getFileSize)(FILE *) = dlsym(handle, "getFileSize");
        // int (*findEmptyIndex)(char **, int) = dlsym(handle, "findEmptyIndex");
        // int (*loadBlock)(char **, char *) = dlsym(handle, "loadBlock");
        int (*loadToMemory)(char **, int, char *) = dlsym(handle, "loadToMemory");
        int (*deleteBlockAtIndex)(char **, int) = dlsym(handle, "deleteBlockAtIndex");
        int (*deleteArray)(char **, int) = dlsym(handle, "deleteArray");
    #endif
    char **tab = NULL;
    int n = 0;
    writeHeader();
    for (int i=1;i<argc;i++){
        char *command = argv[i];
        int status = parseCommand(command);
        if(status==1){
            n = atoi(argv[++i]);
            if (tab) free(tab);
            startTimer();
            tab = createTable(n);
            endTimer();
            saveTimeRaport(command, argv[i]);
        }
        else if(status==2){
            int nextCommandIndex = detectNextCommand(argv, argc, i+1);
            char wc[100] = "wc";
            if(createWcCommand(argv, i, nextCommandIndex, wc)==1){
                startTimer();
                int savedIndex = loadToMemory(tab,n,wc);
                endTimer();
                printf("SAVED AT %d\n", savedIndex);
                int numberOfFiles = nextCommandIndex - (i+1);
                char string[20];
                sprintf(string, "%d", numberOfFiles);
                saveTimeRaport(command, string);
            }
            i=nextCommandIndex-1;
        }
        else if(status==3){
            int index = atoi(argv[++i]);
            if(index >= n)
                printf("ARRAY ISNT THAT LONG\n");
            else{
                startTimer();
                deleteBlockAtIndex(tab, index);
                endTimer();
                saveTimeRaport(command, argv[i]);
            }
        }
        else if(status==4){
            startTimer();
            deleteArray(tab, n);
            endTimer();
            saveTimeRaport(command, "FREE");
        }
        else{
            printf("NO SUCH COMMAND\n");
        }
    }

    // int index = findEmptyIndex(tab, n);
    // printf("%d\n", index);
    // wcOnFiles("wc a.txt b.txt >> tmp.txt");
    // int index = loadToArray(tab, "tmp.txt", 10);
    // printf("%d\n", index);
    // clearFile("tmp.txt");
    // wcOnFiles("wc b.txt >> tmp.txt");
    // index = loadToArray(tab, "tmp.txt", 10);
    // clearFile("tmp.txt");
    // printf("%d\n", index);
    // deleteBlockAtIndex(tab, 0);
    // index = findEmptyIndex(tab, 10);
    // printf("%d\n", index);
    #ifdef dynamic
        dlclose(handle);
    #endif
    return 0;
}