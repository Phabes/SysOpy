#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

char **createTable(int n){
    char **tab = calloc(n, sizeof(char *));
    printf("CREATED\n");
    return tab;
}

void wcFiles(char *wc, char *fileName){
    strcat(wc, fileName);
    system(wc);
    printf("DATA SAVED IN TMP FILE\n");
}

void clearFile(char *fileName){
    fclose(fopen(fileName, "w"));
    printf("TMP FILE CREATED\n");
}

int getFileSize(FILE *fPointer){
    fseek(fPointer, 0L, SEEK_END);
    int size = ftell(fPointer);
    rewind(fPointer);
    printf("FILE SIZE: %d\n", size);
    return size;
}

int findEmptyIndex(char **tab, int size){
    for(int i=0;i<size;i++){
        if(tab[i] == NULL)
            return i;
    }
    printf("NO EMPTY SPACE\n");
    return -1;
}

char *loadBlock(char *fileName){
    FILE *fPointer = fopen(fileName, "r");
    int tmpSize = getFileSize(fPointer);
    char* block = calloc(1, tmpSize);
    fread(block, sizeof(char), tmpSize, fPointer);
    fclose(fPointer);
    return block;
}

int loadToMemory(char **tab, int size, char *wc){
    int index = findEmptyIndex(tab, size);
    if(index!=-1){
        char fileName[] = "tmp.XXXXXX";
        int fd = mkstemp(fileName);
        clearFile(fileName);
        wcFiles(wc, fileName);
        char *block = loadBlock(fileName);
        tab[index] = block;
        close(fd);
        unlink(fileName);
    }
    return index;
}

int deleteBlockAtIndex(char **tab, int index){
    if(tab[index] != NULL){
        free(tab[index]);
        tab[index] = NULL;
        printf("BLOCK DELETED AT %d\n", index);
        return 1;
    }
    return -1;
}

int deleteArray(char **tab, int n){
    for (int i=0;i<n;i++){
        if(tab[i] != NULL){
            free(tab[i]);
            tab[i] = NULL;
        }
    }
    free(tab);
    tab = NULL;
    printf("ARRAY DELETED\n");
    return 1;
}