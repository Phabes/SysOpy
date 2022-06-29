#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

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

void print_element_details(char *elementPath, struct stat *stats, char type[9], int *count){
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
}

void print_dir_details(char *path){
    DIR *dir = opendir(path);
    if(dir == NULL)
        printf("CANT ENTER DIR %s\n", path);
    else{
        struct dirent *element;
        struct stat stats;
        char type[9];
        while ((element = readdir(dir)) != NULL) {
            char elementPath[PATH_MAX];
            strcpy(elementPath, path);
            strcat(elementPath, "/");
            strcat(elementPath, element->d_name);
            // lstat instead of stat to not follow symbolic links
            if(lstat(elementPath, &stats) == 0){
                if(strcmp(element->d_name, ".") != 0 && strcmp(element->d_name, "..") != 0){
                    print_element_details(elementPath, &stats, type, count);
                    if(strcmp(type, "DIR") == 0)
                        print_dir_details(elementPath);
                }
            }
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]){
    if(argc == 2){
        char *path = realpath(argv[1], NULL);
        if(path != NULL){
            count = calloc(7, sizeof(int));
            // add 1 to dir because readdir doesnt check main dir (otherwise you should back one dir in tree and search for that dir)
            int index = get_index("DIR");
            if(index != -1)
                count[index]++;
            print_dir_details(path);
            print_dir_elements_count(path);
            free(count);
        }
        else
            printf("ERROR DURING CONVERTING PATH\n");
    }
    return 0;
}