#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    int i;
    int nfile = argc -1;
    
    int piped[2][2];
    /*piped[0] per i figli di indice pari*/
    /*piped[1] per i i figli di indice dispari*/
    pipe(piped[0]);
    pipe(piped[1]);

    if(argc < 3) {
        printf("Errore: numero parametri sbagliato");
        exit(1);
    }

    int *fd;
    fd = malloc(nfile*sizeof(int));
    for(i = 0; i < nfile; i++) {
        if((fd[i] = open(argv[i + 1], O_RDONLY)) < 0) {
            printf("Errore nell'apertura del file di parametro numero %d", i + 1);
            exit(2);
        }
    }

    int pid;
    for(i = 0; i < nfile; i++) {
        if((pid = fork()) < 0) {
            printf("Errore nella generazione del figlio numero %d", i);
            exit(3);
        }

        if(pid == 0) {
            /*siamo nel figlio*/
            if((i % 2) == 0) {
                /*siamo nel figlio di indice pari*/
                /*associato al file di indice dispari*/

                close(piped[0][0]);
                close(piped[1][0]); /*non serve*/
                close(piped[0][0]); /*chiudiamo il lato di lettura*/

                char c;
                char lastc;
                while(read(fd[i + 1], &c, sizeof(char)) > 0) {
                    if((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) {
                        /*abbiamo trovato un carattere alfabetico*/
                        write(piped[0][1], &c, sizeof(char));
                        /*lo scriviamo sulla pipe*/
                        lastc = c;
                    }
                }

                exit(lastc);
            } else {
                /*siamo nel figlio di indice dispari*/
                /*associato al file di indice pari*/

                close(piped[0][0]);
                close(piped[1][0]); /*non serve*/
                close(piped[1][0]); /*chiudiamo il lato di lettura*/

                char c;
                int lastc;
                while(read(fd[i + 1], &c, sizeof(char)) > 0) {
                    if(c >= 47 && c <= 57) {
                        /*abbiamo trovato un carattere numerico*/
                        write(piped[0][1], &c, sizeof(char));
                        /*lo scriviamo sulla pipe*/
                        lastc = c;
                    }
                }

                exit(lastc);
            }
        }
    }

    /*siamo nel padre*/
    close(piped[0][1]);
    close(piped[1][1]);
    /*chiudiamo i lati di scrittura delle pipe*/
    int totchar = 0;
    char c1;
    char c2;
    while(1) {
        if(read(piped[0][0], &c1, sizeof(char)) > 0) {
            printf("%c", c1);
            totchar++;
        }

        if(read(piped[1][0], &c2, sizeof(char)) > 0) {
            printf("%c", c2);
            totchar++;
        }

        if(read(piped[0][0], &c1, sizeof(char)) <= 0 && read(piped[1][0], &c1, sizeof(char)) <= 0) {
            break;
        }
    }

    int status;
    for(i = 0; i < nfile; i++) {
        if((pid = wait(&status)) < 0) {
            printf("Errore nella wait per il figlio di indice %d", i);
            exit(4);
        }

        if(status & 0XFF) {
            printf("Errore nel figlio di indicie %d", i);
            exit(5);
        }

        int ret = (status >> 8) & 0XFF;
        printf("Terminato figlio con pid %d, ha ritornato %d", pid, ret);
    }

    /*una volta aspettati tutti i figli*/
    printf("Numero totale di caratteri stampati dai figli: %d\n", totchar);

    exit(0);
}