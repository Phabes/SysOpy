#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>


int main(int argc, char* argv[])
{

 if(argc !=2){
    printf("Not a suitable number of program parameters\n");
    exit(1);
 }

 int toChildFD[2];
 int toParentFD[2];

 pipe(toChildFD);
 pipe(toParentFD);

 int val1,val2,val3 = 0;

 pid_t pid;

    if((pid = fork()) == 0) {
       if(close(toChildFD[1]) == -1){
        fprintf(stderr, "err child toChildFD[1]");
        exit(-1);
       }
       if(close(toParentFD[0]) == -1){
        fprintf(stderr, "err child toParentFD[0]");
        exit(-1);
       }
    //odczytaj z potoku nienazwanego wartosc przekazana przez proces macierzysty i zapisz w zmiennej val2
    read(toChildFD[0], &val2, sizeof(int));
   if(close(toChildFD[0]) == -1){
        fprintf(stderr, "err child toChildFD[0]");
        exit(-1);
       }

     val2 = val2 * val2;

   //  //wyslij potokiem nienazwanym val2 do procesu macierzysego
    write(toParentFD[1], &val2, sizeof(int));
   if(close(toParentFD[1]) == -1){
        fprintf(stderr, "err child toParentFD[1]");
        exit(-1);
       }

 } else {
   if(close(toChildFD[0]) == -1){
        fprintf(stderr, "err parent toChildFD[0]");
        exit(-1);
       }
   if(close(toParentFD[1]) == -1){
        fprintf(stderr, "err parent toParentFD[1]");
        exit(-1);
       }
     val1 = atoi(argv[1]);
     
    //wyslij val1 potokiem nienazwanym do priocesu potomnego
    write(toChildFD[1], &val1, sizeof(int));
   if(close(toChildFD[1]) == -1){
        fprintf(stderr, "err parent toChildFD[1]");
        exit(-1);
       }

 
     sleep(1);
     
    //odczytaj z potoku nienazwanego wartosc przekazana przez proces potomny i zapisz w zmiennej val3
    read(toParentFD[0], &val3, sizeof(int));
   if(close(toParentFD[0]) == -1){
        fprintf(stderr, "err parent toParentFD[0]");
        exit(-1);
       }
    
     printf("%d square is: %d\n",val1, val3);
 }
 return 0;
}
