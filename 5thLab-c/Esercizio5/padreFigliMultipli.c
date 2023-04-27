#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Errore: numero parametri sbagliato");
        exit(1);
    }

    int num = atoi(argv[1]);
    if(num < 0 || num >= 255) {
        printf("Errore: il secondo parametro deve essere un numero positivo e minore di 255");
        exit(2);
    }

    printf("ID del processo corrente: %d, numero immesso come parametro: %d\n", getpid(), num);

    int i;
    for(i = 0; i < num; i++) {
        int forkret = fork();
        if(forkret < 0) {
            printf("Errore nella generazione del processo %d\n", i);
            exit(3);
        }

        if(forkret == 0) {
            printf("Stampa del processo figlio numero %d, il cui pid è %d\n", i, getpid());
            exit(i);
        }

        int status;
        wait(&status);
        int ret = status>>8;
        ret &= 0XFF;
        printf("ID del processo figlio: %d, suo valore di ritorno: %d\n", forkret, ret);
    }

    /* for(i = 0; i < num; i++) {
        int status;
        int forkret = wait(&status);
        int ret = status>>8;
        ret &= 0XFF; 
        printf("ID del processo figlio: %d, suo valore di ritorno: %d\n", forkret, ret);
    } */
}