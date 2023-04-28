#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    if(argc < 4) {
        printf("Errore: numero parametri sbagliato");
        exit(1);
    }

    int numfile = argc - 2;
    int i ;
    for(i = 1; i < numfile; i++) {
        if(open(argv[i], O_RDONLY) < 0) {
            printf("Errore nell'apertura del file passato come parametro numero %d", i);
            exit(2);
        }
    }

    if(argv[argc - 1][1] != 0) {
        printf("Errore: l'ultimo parametro non è un singolo carattere");
        exit(3);
    }
    char target = argv[argc - 1][0];

    for(i = 0; i < numfile; i++) {
        int pid;
        if((pid = fork()) < 0) {
            printf("Errore nella creazione di un processo figlio");
            exit(4);
        }

        char c;
        int count = 0;
        if(pid == 0) {
            int fd = open(argv[i + 1], O_RDONLY);

            while(read(fd, &c, sizeof(char)) > 0) {
                if(c == target) {
                    count++;
                }
            }
            exit(count);
        }
    }

    int status;
    for(i = 0; i < numfile; i++) {
        int waitret = wait(&status);

        if((status & 0XFF) != 0) {
            printf("Errore nella wait");
            exit(5);
        }
        int ret = (status >> 8) & 0XFF;

        printf("Il processo %d ha trovato %d occorrenze del carattere %c nel file associato ad esso", waitret, ret, target);
    }

    exit(0);
}