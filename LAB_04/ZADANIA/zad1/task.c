#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define TESTING_SIGNAL SIGUSR1

void ignore_signal(){
    printf("RAISING CHILD: %d\n", getpid());
    raise(TESTING_SIGNAL);
    printf("INGORED CHILD: %d\n", getpid());
}

void mask_signal(){
    printf("RAISING CHILD: %d\n", getpid());
    raise(TESTING_SIGNAL);
    printf("MASKED CHILD: %d\n", getpid());
}

void pending_signal(){
    sigset_t pending_mask;
    printf("RAISING CHILD: %d\n", getpid());
    sigpending(&pending_mask);
    if(sigismember(&pending_mask, TESTING_SIGNAL) == 1)
        printf("Signal is pending in child\n");
    else
        printf("Signal isnt pending in child\n");
    printf("PENDING CHILD: %d\n", getpid());
}

int main(int argc, char *argv[]){
    if(argc == 2){
        char *param = argv[1];
        if(strcmp(param, "ignore") == 0)
            ignore_signal();
        else if(strcmp(param, "mask") == 0)
            mask_signal();
        else if(strcmp(param, "pending") == 0)
            pending_signal();
        else
            printf("WRONG PARAMETER VALUE\n");
    }
    else
        printf("WRONG NUMBER OF ARGUMENTS\n");
    int status;
    // wait until all processes done
	while((wait(&status)) >= 0);
    return 0;
}