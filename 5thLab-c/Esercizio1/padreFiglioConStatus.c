#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>

int mia_random(int n) {
    int casuale;
    casuale = rand() % n;
    return casuale;
}

int main(int argc, char* argv[]) {
    int pid = getpid();
    printf("%d\n", pid);

    int forkret = fork();

    if(forkret < 0) {
        printf("Errore nel fork: non è stato possibile creare il processo figlio\n");
        exit(1);
    }

    if(forkret == 0) {
        pid = getpid();
        printf("%d\n", pid);
        int ppid = getppid();
        printf("%d\n", ppid);

        srand(time(NULL));
        int rnd = mia_random(100);
        exit(rnd);
    }

    int status;
    int waitret = wait(&status);

    if(waitret < 0) {
        printf("Errore nella wait\n");
        exit(2);
    }

    if((status & 0XFF) != 0) {
        printf("Errore nell'esecuzione del figlio\n");
        exit(3);
    }

    int ret = status>>8;
    ret &= 0XFF;

    printf("%d\n", ret);  

    exit(0);
}