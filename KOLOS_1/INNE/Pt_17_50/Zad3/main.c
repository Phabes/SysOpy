#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


/*
 * Funkcja 'spawn_child' powinna wpierw utworzyc proces
 * potomny. Proces macierzysty powinien nastepnie:
 *   - skopiowac deskryptor plikow 'out_fd' na deskryptor
 *     standardowego wyjscia,
 *   - zamknac deskryptor 'out_fd'.
 *
 * Proces potomny powinien:
 *   - skopiowac deskryptor plikow 'in_fd' na deskryptor
 *     standardowego wejscia,
 *   - zamknac deskryptor 'in_fd'.
 *
 * W procesie potomnym funkcja 'spawn_child' powinna
 * zwrocic wartosc 0. W procesie macierzystym funkcja
 * 'spawn_child' powinna zwrocic PID potomka.
 *
 */
pid_t spawn_child(int in_fd, int out_fd){
   // Uzupelnij cialo funkcji spawn_child zgodnie z
   // komentarzem powyzej
    pid_t child = fork();
    if(child == 0){
        if(dup2(in_fd, STDIN_FILENO) == -1){
            fprintf(stderr, "dup2 in_fd error");
            exit(-1);
        }
        if(close(in_fd) == -1){
            fprintf(stderr, "close in_fd error");
            exit(-1);
        }
        return 0;
    }
    else{
        if(dup2(out_fd, STDOUT_FILENO) == -1){
            fprintf(stderr, "dup2 out_fd error");
            exit(-1);
        }
        if(close(out_fd) == -1){
            fprintf(stderr, "close out_fd error");
            exit(-1);
        }
        return child;
    }
}


int main(void) {
    int fds[2];

    srand(0);
    pipe(fds);
    pid_t pid = spawn_child(fds[0], fds[1]);
    if(pid){
        if(fcntl(fds[1], F_GETFD) != -1) return 0;
        printf("spawned child %d\n", pid);
        printf("magic = %d\n", rand() % 8192);
    } else {
        if(fcntl(fds[0], F_GETFD) != -1) return 0;
        printf("Child: my pid %d\n", getpid());
        char *line = NULL;
        size_t n = getline(&line, &n, stdin);
        printf("Received from parent:\t%s", line);
        n = getline(&line, &n, stdin);
        printf("Received from parent:\t%s", line);
        free(line);
    }

    return 0;
}
