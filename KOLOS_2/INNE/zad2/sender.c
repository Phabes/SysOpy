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

    if (argc != 2) {
        printf("Not a suitable number of program parameters\n");
        return (1);
    }
    /***************************************
    Utworz/otworz posixowy obszar pamieci wspolnej "reprezentowany" przez SHM_NAME
    zapisz tam wartosc przekazana jako parametr wywolania programu
    posprzataj
    *****************************************/
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
    int* t = mmap(NULL, MAX_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, desc, 0);
    if(t == MAP_FAILED){
        printf("MMAP ERROR\n");
        exit(0);
    }
    *t = atoi(argv[1]);
    int unmap = munmap(t, MAX_SIZE);
    if(unmap == -1){
        printf("MUNMAP ERROR\n");
        exit(0);
    }
    return 0;
}
