#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1

void execute_commands(FILE *read_file){
    char **commands = (char **)calloc(10, sizeof(char(*)));
    int current_index = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t line_size;

    while ((line_size = getline(&line, &len, read_file)) != -1){
        if(strstr(line, "=")){
            char *line_to_save = (char *)calloc(line_size, sizeof(char));
            char *definition = strtok(line, "=");
            definition = strtok(NULL, "");
            size_t ln = strlen(definition) - 1;
            if (*definition && definition[ln] == '\n') 
                definition[ln] = '\0';
            strcpy(line_to_save, definition);
            commands[current_index++] = line_to_save;
        }
        else{
            size_t ln = strlen(line) - 1;
            if (*line && line[ln] == '\n') 
                line[ln] = '\0';
            printf("LINE: %s\n", line);
            int *indexes = calloc(10, sizeof(int));
            int count = 0;
            char *current_command = strtok(line, "|");
            while( current_command != NULL ) {
                int start = 7;
                if(count > 0)
                    start++;
                char* index_str;
                index_str = (char*)malloc(5);
                // for (int i = start; i < strlen(current_command) - 1; i++) {
                //     printf("CURR: %c\n", current_command[i]);
                //     index_str[i - start] = current_command[i];
                // }
                index_str[0] = current_command[start];
                int index = atoi(index_str);
                indexes[count++] = index;
                current_command = strtok(NULL, "|");
            }
            printf("COUNT: %d\n", count);
            for (int i = 0; i < count; i++){
                char **single_tasks = (char **)calloc(10, sizeof(char *));
                int count2 = 0;
                printf("INDEX: %d\n", indexes[i]);
                printf("COMMAND: %s\n", commands[indexes[i]]);
                current_command = strtok(commands[indexes[i]], "|");
                while(current_command != NULL) {
                    single_tasks[count2++] = current_command;
                    current_command = strtok(NULL, "|");
                }
                int pipes[10][2];
                for (int j = 0; j < count2 - 1; j++){
                    if (pipe(pipes[j]) < 0){
                        fprintf(stderr, "cant make pipe\n");
                        exit(1);
                    }
                }

                for (int j = 0; j < count2; j++)
                {
                    pid_t pid = fork();
                    if (pid < 0){
                        fprintf(stderr, "cant fork\n");
                        exit(1);
                    }
                    else if (pid == 0){
                        if (j > 0)
                            dup2(pipes[j - 1][0], STDIN_FILENO);
                        if (j + 1 < count2)
                            dup2(pipes[j][1], STDOUT_FILENO);
                        for (int k = 0; k < count2 - 1; k++)
                        {
                            close(pipes[k][0]);
                            close(pipes[k][1]);
                        }
                        char *command_element = strtok(single_tasks[j], " ");
                        char path[100];
                        strcpy(path, command_element);
                        printf("PATH: %s\n", path);
                        char **args = (char **)calloc(10, sizeof(char *));
                        int count3 = 0;
                        while(command_element != NULL) {
                            printf("ARG: %s\n", command_element);
                            args[count3++] = command_element;
                            command_element = strtok(NULL, " ");
                        }
                        execvp(path, args);
                    }
                }
                for (int j = 0; j < count2 - 1; ++j)
                {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
                for (int j = 0; j < count2; ++j)
                    wait(0);
            }
        }
        // wait for all the child processes to terminate
        int status = 0;
        pid_t wpid;
        while ((wpid = wait(&status)) != -1);
    }
}

int main(int argc, char *argv[]){
    if (argc == 2){
        FILE *read_file = fopen(argv[1], "r");
        if(read_file == NULL){
            printf("CANT READ FILE\n");
        }
        else{
            execute_commands(read_file);
            fclose(read_file);
        }
    }
    else
        printf("WRONG NUMBER OF PARAMETERS\n");
    return 0;
}