#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include <unistd.h>
#include <sys/mman.h>

#define SHM_NAME "/kol_shm"
#define MAX_SIZE 1024

int main(int argc, char **argv) {

    sleep(1);
    int val = 0;
    /*******************************************
    Utworz/otworz posixowy obszar pamieci wspolnej "reprezentowany" przez SHM_NAME
    odczytaj zapisana tam wartosc i przypisz ja do zmiennej val
    posprzataj
    *********************************************/
    int desc = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
    if(desc == -1){
        printf("SHM_OPEN ERROR\n");
        exit(0);
    }
    int trun = ftruncate(desc, MAX_SIZE);
    if(trun == -1){
        printf("FTRUNCATE ERROR\n");
        exit(0);
    }
    int* tmp = mmap(NULL, MAX_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, desc, 0);
    if(tmp == MAP_FAILED){
        printf("MMAP ERROR\n");
        exit(0);
    }
    val = *tmp;
    int unmap = munmap(tmp, MAX_SIZE);
    if(unmap == -1){
        printf("MUNMAP ERROR\n");
        exit(0);
    }
    int unlink = shm_unlink(SHM_NAME);
    if(unlink == -1){
        printf("SHM_UNLINK ERROR\n");
        exit(0);
    }
    printf("%d square is: %d \n", val, val * val);
    return 0;
}
