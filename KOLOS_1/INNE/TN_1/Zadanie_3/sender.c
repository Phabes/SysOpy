#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#define PIPE "./squareFIFO"

int main(int argc, char* argv[]) {

 if(argc !=2){
   printf("Not a suitable number of program parameters\n");
   return(1);
 } 

  //utworz potok nazwany pod sciezka reprezentowana przez PIPE
  //zakladajac ze parametrem wywolania programu jest liczba calkowita
  //zapisz te wartosc jako int do potoku i posprzataj
  int fifo = mkfifo(PIPE, 0777);
  FILE *fifo_file = fopen(PIPE, "w");
  fwrite(argv[1], sizeof(int), 1, fifo_file);
  fclose(fifo_file);
    return 0;
}
