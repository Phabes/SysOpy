#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define TESTING_SIGNAL SIGUSR1

void ignore_signal(){
    struct sigaction signal;
    signal.sa_handler = SIG_IGN; // comment this line and signal wont be ignored
    sigaction(TESTING_SIGNAL, &signal, NULL);
    printf("RAISING PARENT: %d\n", getpid());
    raise(TESTING_SIGNAL);
    printf("INGORED PARENT: %d\n", getpid());
    execl("./task", "task", "ignore", NULL);
}

void mask_signal(){
    sigset_t new_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, TESTING_SIGNAL); // comment this line and signal wont be masked
    if(sigprocmask(SIG_BLOCK, &new_mask, NULL) < 0)
        printf("Didnt block\n");
    printf("RAISING PARENT: %d\n", getpid());
    raise(TESTING_SIGNAL);
    printf("MASKED PARENT: %d\n", getpid());
    execl("./task", "task", "mask", NULL);
}

void pending_signal(){
    sigset_t new_mask;
    sigset_t pending_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, TESTING_SIGNAL);
    if(sigprocmask(SIG_BLOCK, &new_mask, NULL) == -1)
        printf("Didnt block\n");
    printf("RAISING PARENT: %d\n", getpid());
    raise(TESTING_SIGNAL); // if not commented child will be pending
    sigpending(&pending_mask);
    if(sigismember(&pending_mask, TESTING_SIGNAL) == 1)
        printf("Signal is pending in parent\n");
    else
        printf("Signal isnt pending in parent\n");
    printf("PENDING PARENT: %d\n", getpid());
    execl("./task", "task", "pending", NULL);
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