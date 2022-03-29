#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int is_number(char *number){
    int length = strlen(number);
    for(int i = 0; i < length; i++){
        if(isdigit(number[i]) == 0)
            return 0;
    }
    return 1;
}

void create_process(){
    pid_t process = fork();
    if(process == 0){
        printf("String from: %d\n", (int)getpid());
        exit(0);
    }
}

int main(int argc, char *argv[]){
    if(argc == 2){
        if(is_number(argv[1]) == 0)
            printf("PARAMETER IS NOT NUMBER\n");
        else{
            int n = atoi(argv[1]);
            printf("Main process: %d\n", (int)getpid());
            for(int i = 0; i < n; i++)
                create_process();
        }
    }
    else
        printf("WRONG NUMBER OF PARAMETERS\n");
    int status;
    // wait until all processes done
	while((wait(&status)) >= 0);
    return 0;
}