#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

int count = 0;
int number_of_signals;
int send_signal;
int end_signal;

int is_number(char *number){
    int length = strlen(number);
    for(int i = 0; i < length; i++){
        if(isdigit(number[i]) == 0)
            return 0;
    }
    return 1;
}

int is_type_send(char *type){
    if(strcmp(type, "KILL") == 0)
        return 1;
    if(strcmp(type, "SIGQUEUE") == 0)
        return 2;
    if(strcmp(type, "SIGRT") == 0)
        return 3;
    return 0;
}

void kill_type(pid_t catcher_pid, int number_of_signals){
    for(int i = 0; i < number_of_signals; i++)
        kill(catcher_pid, send_signal);
    kill(catcher_pid, end_signal);
}

void sigqueue_type(pid_t catcher_pid, int number_of_signals){
    union sigval message;
    for(int i = 0; i < number_of_signals; i++){
        message.sival_int = i;
        sigqueue(catcher_pid, send_signal, message);
    }
    sigqueue(catcher_pid, end_signal, message);
}

void sigrt_type(pid_t catcher_pid, int number_of_signals){
    for(int i = 0; i < number_of_signals; i++)
        kill(catcher_pid, send_signal);
    kill(catcher_pid, end_signal);
}

void receive_signal(int signal_number){
    count++;
}

void receive_signal_sigqueue(int signal_number, siginfo_t *info, void *ucontext){
    count++;
    printf("RECEIVED FOR NOW: %d, INDEX %d\n", count, info->si_value.sival_int);
}

void end_receiving(int signal_number){
    printf("RECEIVED: %d, EXPECTED %d\n", count, number_of_signals);
    exit(0);
}

int main(int argc, char *argv[]){
    if(argc == 4){
        int type;
        if(is_number(argv[1]) == 1 && is_number(argv[2]) == 1 && (type = is_type_send(argv[3])) != 0){
            pid_t catcher_pid = (pid_t) atoi(argv[1]);
            number_of_signals = atoi(argv[2]);
            printf("SENDER PID: %d, TYPE: %s\n", getpid(), argv[3]);
            struct sigaction sender_usr1;
            struct sigaction sender_usr2;
            if(type == 1){
                send_signal = SIGUSR1;
                end_signal = SIGUSR2;
                sender_usr1.sa_handler = &receive_signal;
                sigaction(send_signal, &sender_usr1, NULL);
                sender_usr2.sa_handler = &end_receiving;
                sigaction(end_signal, &sender_usr2, NULL);
                kill_type(catcher_pid, number_of_signals);
            }
            else if(type == 2){
                send_signal = SIGUSR1;
                end_signal = SIGUSR2;
                sender_usr1.sa_sigaction = &receive_signal_sigqueue;
                sender_usr1.sa_flags = SA_SIGINFO;
                sigaction(send_signal, &sender_usr1, NULL);
                sender_usr2.sa_handler = &end_receiving;
                sigaction(end_signal, &sender_usr2, NULL);
                sigqueue_type(catcher_pid, number_of_signals);
            }
            else if(type == 3){
                send_signal = SIGRTMIN;
                end_signal = SIGRTMIN + 1;
                sender_usr1.sa_handler = &receive_signal;
                sigaction(send_signal, &sender_usr1, NULL);
                sender_usr2.sa_handler = &end_receiving;
                sigaction(end_signal, &sender_usr2, NULL);
                sigrt_type(catcher_pid, number_of_signals);
            }
            sigset_t sender_mask;
            sigfillset(&sender_mask);
            sigdelset(&sender_mask, send_signal);
            sigdelset(&sender_mask, end_signal);
            sigprocmask(SIG_BLOCK, &sender_mask, NULL);
            for(;;)
                sigsuspend(&sender_mask);
        }
        else
            printf("WRONG PARAMS\n");
    }
    else
        printf("WRONG NUMBER OF PARAMETERS\n");
    return 0;
}