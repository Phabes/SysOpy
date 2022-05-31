#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int count = 0;
int send_signal;
int end_signal;
int type;

int is_type_send(char *type){
    if(strcmp(type, "KILL") == 0)
        return 1;
    if(strcmp(type, "SIGQUEUE") == 0)
        return 2;
    if(strcmp(type, "SIGRT") == 0)
        return 3;
    return 0;
}

void send_back(pid_t sender_pid){
    if(type != 2){
        for(int i = 0; i < count; i ++)
            kill(sender_pid, send_signal);
        kill(sender_pid, end_signal);
    }
    else{
        union sigval message;
        for(int i = 0; i < count; i++){
            message.sival_int = i;
            sigqueue(sender_pid, send_signal, message);
        }
        sigqueue(sender_pid, end_signal, message);
    }
}

void count_messages(int signal_number){
    count++;
}

void end_receiving(int signal_number, siginfo_t *info, void *ucontext){
    send_back(info->si_pid);
    printf("RECEIVED: %d, SENDER PID: %d\n", count, info->si_pid);
    exit(0);
}

int main(int argc, char *argv[]){
    if(argc == 2){
        if((type = is_type_send(argv[1])) != 0){
            printf("CATCHER PID: %d\n", getpid());
            if(type != 3){
                send_signal = SIGUSR1;
                end_signal = SIGUSR2;
            }
            else{
                send_signal = SIGRTMIN;
                end_signal = SIGRTMIN + 1;
            }
            struct sigaction catcher_usr1;
            catcher_usr1.sa_handler = &count_messages;
            sigaction(send_signal, &catcher_usr1, NULL);
            struct sigaction catcher_usr2;
            catcher_usr2.sa_sigaction = &end_receiving;
            catcher_usr2.sa_flags = SA_SIGINFO;
            sigaction(end_signal, &catcher_usr2, NULL);
            sigset_t catcher_mask;
            sigfillset(&catcher_mask);
            sigdelset(&catcher_mask, send_signal);
            sigdelset(&catcher_mask, end_signal);
            sigprocmask(SIG_BLOCK, &catcher_mask, NULL);
            for(;;)
                sigsuspend(&catcher_mask);
        }
        else
            printf("WRONG PARAMS\n");
    }
    else
        printf("WRONG NUMBER OF PARAMETERS\n");
    return 0;
}