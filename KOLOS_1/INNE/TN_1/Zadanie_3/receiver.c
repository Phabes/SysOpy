#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define PIPE "./squareFIFO"

int main() {
    int val = 0;
    /***********************************
    * odczytaj z potoku nazwanego PIPE zapisana tam wartosc i przypisz ja do zmiennej val
    * posprzataj
    ************************************/
   FILE *fifo_file = fopen(PIPE, "r");
   char buff[10];
   fread(buff, sizeof(int), 1, fifo_file);
    val = atoi(buff);
    printf("%d square is: %d\n", val, val*val);
    fclose(fifo_file);
    return 0;
}
