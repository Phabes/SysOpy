#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define TESTING_SIGNAL SIGUSR1

void handler(int signal_number, siginfo_t *info, void *ucontext){
    printf("SIG: %d, PID: %d\n", info->si_signo, info->si_pid);
    printf("ADDITIONAL INFO:\n");
    printf("An errno value: %d\n", info->si_errno); // 0 means no error
    printf("Signal code: %d\n", info->si_code); // probably -6 means send by tkill
    printf("Real user ID of sending process: %d\n", info->si_uid); // shows user id who runs program
}

void single_handler(int signal_number){
    printf("SINGLE HANDLER - SIG: %d, PID: %d\n", signal_number, getpid());
}

void test_sa_siginfo(){
    struct sigaction signal;
    signal.sa_sigaction = &handler;
    signal.sa_flags = SA_SIGINFO;
    sigaction(TESTING_SIGNAL, &signal, NULL);
    raise(TESTING_SIGNAL); // the same as kill(getpid(), TESTING_SIGNAL)
}

void test_sa_resethand(){
    struct sigaction signal;
    signal.sa_handler = &single_handler;
    sigaction(SIGCHLD, &signal, NULL);
    pid_t child_pid;
    for(int i = 0; i < 3; i++){
        if((child_pid = fork()) == 0)
            for(;;);
        else{
            printf("Kill %d child\n", i);
            kill(child_pid, SIGKILL); // default signal action wont be restored so hadler will be working for all "i"
            wait(NULL);
        }
    }
    printf("\n");
    signal.sa_flags = SA_RESETHAND;
    sigaction(SIGCHLD, &signal, NULL);
    for(int i = 0; i < 3; i++){
        if((child_pid = fork()) == 0)
            for(;;);
        else{
            printf("Kill %d child\n", i);
            kill(child_pid, SIGKILL); // after first call handler will be ignored because default signal action will be restored
            wait(NULL);
        }
    }
}

void test_sa_nocldstop(){
    struct sigaction signal;
    signal.sa_handler = &single_handler;
    sigaction(SIGCHLD, &signal, NULL);
    pid_t child_pid;
    if((child_pid = fork()) == 0)
        while(1);
    else{
        printf("Stop child\n");
        kill(child_pid, SIGSTOP);
        wait(NULL);
    }
    printf("\n");
    signal.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &signal, NULL);
    if((child_pid = fork()) == 0)
        while(1);
    else{
        printf("Stop child\n");
        kill(child_pid, SIGSTOP);
        // wait(NULL); // if not commented program will wait for information about STOP but wont receive it because of SA_NOCLDSTOP flag
    }
}

void test_flags(){
    test_sa_siginfo();
    test_sa_resethand();
    test_sa_nocldstop();
}

int main(int argc, char *argv[]){
    test_flags();
    return 0;
}