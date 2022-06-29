#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_emails(char *mode){
    char *command_to_execute;
    if(strcmp(mode, "data") == 0)
        command_to_execute = "echo | mail -H | tac";
    else if(strcmp(mode, "nadawca") == 0)
        command_to_execute = "echo | mail -H | sort -k 3";
    else{
        printf("Wrong parameter\n");
        exit(1);
    }
    FILE *f = popen(command_to_execute, "r");
    if(f == NULL){
        printf("popen failed");
        exit(1);
    }
    char *line = NULL;
    size_t len, line_size;
    while ((line_size = getline(&line, &len, f)) != -1)
        printf("%s", line);
    pclose(f);
}

void send_email(char *address, char *title, char *content){
    char command_to_execute[256];
    snprintf(command_to_execute, sizeof(command_to_execute), "echo %s | mail -s %s %s", content, title, address);
    FILE *f = popen(command_to_execute, "r");
    if(f == NULL){
        printf("popen failed\n");
        exit(1);
    }
    pclose(f);
    printf("Email was sent\n");
}

int main(int argc, char *argv[]){
    if (argc == 2)
        print_emails(argv[1]);
    else if(argc == 4)
        send_email(argv[1],argv[2],argv[3]);
    else
        printf("WRONG NUMBER OF PARAMETERS\n");
    return 0;
}