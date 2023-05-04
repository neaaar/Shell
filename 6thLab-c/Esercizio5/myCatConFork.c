#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Errore: inserire un solo parametro");
        exit(1);
    }

    if((open(argv[1], O_RDONLY)) < 0) {
        printf("Errore nell'apertura del file");
        exit(2);
    }

    int pid;
    if((pid = fork()) < 0) {
        printf("Errore nella fork");
        exit(3);
    }

    if(pid == 0) {
        close(0);
        open(argv[1], O_RDONLY);
        execlp("cat", "cat", (char *)0);

        exit(-1);
    }

    int status;
    int retpid = wait(&status);

    if(retpid < 0) {
        printf("Errore nella wait");
        exit(4);
    }

    if((status & 0XFF) != 0) {
        printf("Errore nel processo figlio");
        exit(5);
    }

    int ret = (status >> 8) & 0XFF;
    printf("Il processo figlio con pid %d ha ritornato il valore %d (se 255 errore)", retpid, ret);

    exit(0);
}