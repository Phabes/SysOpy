#include <stdio.h>
#ifndef LAB1_LIB_H
#define LAB1_LIB_H
#pragma once

char **createTable(int n);
void wcFiles(char *wc, char *fileName);
void clearFile(char *fileName);
int getFileSize(FILE *fPointer);
int findEmptyIndex(char **tab, int size);
char *loadBlock(char *fileName);
int loadToMemory(char **tab, int size, char *wc);
int deleteBlockAtIndex(char **tab, int index);
int deleteArray(char **tab, int n);

#endif //LAB1_LIB_H