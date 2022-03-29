#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <ctype.h>

int is_number(char *number){
    int length = strlen(number);
    for(int i = 0; i < length; i++){
        if(isdigit(number[i]) == 0)
            return 0;
    }
    return 1;
}

int is_directory(char *path) {
    struct stat stats;
    if (lstat(path, &stats) != 0)
        return 0;
    return S_ISDIR(stats.st_mode);
}

int is_txt(struct dirent *file){
    char *extension = strrchr(file->d_name, '.');
    if(extension != NULL && strcmp(extension, ".txt") == 0)
        return 1;
    return 0;
}

void search_in_dir(char *path, char *stringToFind, char *originalPath, int depth){
    DIR *dir = opendir(path);
    if(dir == NULL)
        printf("CANT ENTER DIR %s\n", path);
    else{
        struct dirent *element;
        while ((element = readdir(dir)) != NULL) {
            if(strcmp(element->d_name, ".") != 0 && strcmp(element->d_name, "..") != 0){
                char elementPath[strlen(path) + strlen(element->d_name) + 1];
                snprintf(elementPath, strlen(path) + strlen(element->d_name) + 2, "%s/%s", path, element->d_name);
                if(is_directory(elementPath) != 0){
                    if(depth > 0){
                        if(fork() == 0){
                            search_in_dir(elementPath, stringToFind, originalPath, depth - 1);
                            exit(0);
                        }
                    }
                }
                else{
                    if(is_txt(element)){
                        FILE *readFile = fopen(elementPath, "r");
                        if(readFile == NULL)
                            printf("CANT ENTER FILE\n");
                        else{
                            // second method is commented
                            char sign;
                            int length = strlen(stringToFind);
                            int index = 0;
                            // int startIndex;
                            // int currentIndex = 0;
                            fpos_t position;
                            while (fread(&sign, sizeof(char), 1, readFile) == 1){
                                if(index == length){
                                    int pathLength = strlen(elementPath);
                                    printf("PATH, PID: ");
                                    for(int i = strlen(originalPath); i < pathLength; i++)
                                        printf("%c", elementPath[i]);
                                    printf(" - %d\n", (int)getpid());
                                    break;
                                }
                                if(sign == stringToFind[index]){
                                    if(index == 0){
                                        // startIndex = currentIndex;
                                        fgetpos(readFile, &position);
                                    }
                                    index++;
                                }
                                else{
                                    if(index != 0){
                                        index = 0;
                                        fsetpos(readFile, &position);
                                        // fseek(readFile, startIndex + 1, 0);
                                        // currentIndex = startIndex;
                                    }
                                }
                                // currentIndex++;
                            }
                        }
                        fclose(readFile);
                    }
                }
            }
        }
    }
    closedir(dir);
    int status;
	while(wait(&status) >= 0);
    exit(0);
}

int main(int argc, char *argv[]){
    if(argc == 4){
        if(is_number(argv[3]) == 1){
            if(is_directory(argv[1]) != 0){
                if(fork() == 0)
                    search_in_dir(argv[1], argv[2], argv[1], atoi(argv[3]));
            }
            else
                printf("GIVEN PATH IS NOT DIR\n");
        }
        else
            printf("LAST ARGUMENT IS NOT NUMBER\n");
    }
    int status;
	while(wait(&status) >= 0);
    return 0;
}