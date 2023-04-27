#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Errore: numero parametri sbagliato\n");
        exit(1);
    }

    int num = atoi(argv[1]);
    if(num < 0 || num >= 155) {
        printf("Errore: il parametro deve essere un numero positivo e minore di 155\n");
        exit(2);
    }

    printf("ID del processo corrente: %d, numero passato come parametro: %d\n", getpid(), num);

    int i;
    for(i = 0; i < num; i++) {
        int forkret = fork();
        if(forkret < 0) {
            printf("Errore nella generazione del processo %d\n", i);
            exit(3);
        }

        if(forkret == 0) {
            printf("Stampa del processo figlio numero %d, il cui pid è %d\n", i, getpid());

            srand(time(NULL));
            int rnd = rand() % (100 + i);
            exit(rnd);
        }
    }
    
    exit(0);
}