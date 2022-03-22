#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <ftw.h>

const char *types[] = {"FILES", "DIRS", "CHAR DEVS", "BLOCK DEVS", "FIFOS", "SLINKS", "SOCKS"};
const char format[] = "%Y-%m-%d %H:%M:%S";
int *count;

void detect_element_type(char type[9], mode_t mode){
    if(S_ISREG(mode) != 0)
        strcpy(type, "FILE");
    else if(S_ISDIR(mode) != 0)
        strcpy(type, "DIR");
    else if(S_ISCHR(mode) != 0)
        strcpy(type, "CHAR DEV");
    else if(S_ISBLK(mode) !=0)
        strcpy(type, "BLOCK DEV");
    else if(S_ISFIFO(mode) != 0)
        strcpy(type, "FIFO");
    else if(S_ISLNK(mode) != 0)
        strcpy(type, "SLINK");
    else if(S_ISSOCK(mode) != 0)
        strcpy(type, "SOCK");
    else
        strcpy(type, "ERROR");
}

int get_index(char *type){
    if(strcmp(type, "FILE") == 0)
        return 0;
    if(strcmp(type, "DIR") == 0)
        return 1;
    if(strcmp(type, "CHAR DEV") == 0)
        return 2;
    if(strcmp(type, "BLOCK DEV") == 0)
        return 3;
    if(strcmp(type, "FIFO") == 0)
        return 4;
    if(strcmp(type, "SLINK") == 0)
        return 5;
    if(strcmp(type, "SOCK") == 0)
        return 6;
    return -1;
}

void print_dir_elements_count(char *dirPath){
    printf("DIR PATH: %s\n", dirPath);
    for(int i=0; i<7; i++)
        printf("%s: %d\n", types[i], count[i]);
    printf("\n");
}

void convert_time(time_t time, struct tm timeStruct, char timeString[255]){
    localtime_r(&time, &timeStruct);
    strftime(timeString, 255, format, &timeStruct);
}

int element_info(const char *elementPath, const struct stat *stats, int fileFlags, struct FTW *pftw){
    char type[9];
    detect_element_type(type, stats->st_mode);
    int index = get_index(type);
    if(index != -1)
        count[index]++;
    struct tm time;
    char accessTime[255];
    char modTime[255];
    convert_time(stats->st_atime, time, accessTime);
    convert_time(stats->st_mtime, time, modTime);
    printf("ELEMENT PATH: %s\n", elementPath);
    printf("LINKS: %ld\n", stats->st_nlink);
    printf("TYPE: %s\n", type);
    printf("SIZE: %ld[B]\n", stats->st_size);
    printf("LAST ACCESS: %s\n", accessTime);
    printf("LAST MODIFICATION: %s\n", modTime);
    printf("\n");
    return 0;
}

void print_dir_details(char *dirPath){
    count = calloc(7, sizeof(int));
    int flags = FTW_PHYS;
    int limit = 1;
    int dir = nftw(dirPath, element_info, limit, flags);
    if(dir == -1)
        printf("ERROR DURING READING TREE\n");
    print_dir_elements_count(dirPath);
    free(count);
}

int main(int argc, char *argv[]){
    if(argc == 2){
        char *path = realpath(argv[1], NULL);
        if(path != NULL)
            print_dir_details(path);
        else
            printf("ERROR DURING CONVERTING PATH\n");
    }
    return 0;
}