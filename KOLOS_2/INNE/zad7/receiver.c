#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define SOCK_PATH "sock_path"

int main() {
    int fd = -1;
    /*********************************************
    Utworz socket domeny unixowej typu datagramowego
    Utworz strukture adresowa ustawiajac adres/sciezke komunikacji na SOCK_PATH
    Zbinduj socket z adresem/sciezka SOCK_PATH
    **********************************************/
    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(fd == -1){
        printf("SOCKET ERROR\n");
        exit(0);
    }
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    strcpy(addr.sun_path, SOCK_PATH);
    addr.sun_family = AF_UNIX;
    int binding = bind(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un));
    if(binding == -1){
        printf("BIND ERROR\n");
        exit(0);
    }

    char buf[20];
    if (read(fd, buf, 20) == -1)
        perror("Error receiving message");
    int val = atoi(buf);
    printf("%d square is: %d\n", val, val * val);
    /***************************
    Posprzataj po sockecie
    ****************************/
    close(fd);
    unlink(SOCK_PATH);
    return 0;
}

