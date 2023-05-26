#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

typedef int pipe_t[2];

int main(int argc, char* argv[]) {
    int i; //indice per cicli for

    /*controllo sul numero dei parametri*/
    if(argc < 3) {
        printf("Errore: il numero minimo di parametri è 2, ma ne sono stati passati %d\n", argc - 1);
        exit(1);
    }

    char c; //per salvare l'ultimo parametro
    int N; //per salvare il numero di file
    if(argv[argc - 1][1] != 0) {
        printf("Errore: ultimo parametro non singolo carattere\n");
        exit(2);
    }
    /*se controlli passati, salviamo c e N file*/
    c = argv[argc - 1][0];
    N = argc - 2;

    pipe_t* pipedfatherson; //padre scrittore, figli lettori
    pipe_t* pipedsonfather; //figli scrittori, padre lettore
    /*allochiamo memoria per i vettori di pipe*/
    pipedfatherson = malloc(N*sizeof(pipe_t));
    pipedsonfather = malloc(N* sizeof(pipe_t));
    /*creiamo le pipe*/
    for(i = 0; i < N; i++) {
        if(pipe(pipedfatherson[i]) < 0) {
            printf("Errore nella creazione della pipe da padre a figlio numero %d\n", i);
            exit(3);
        }

        if(pipe(pipedsonfather[i]) < 0) {
            printf("Errore nella creazione della pipe da figlio numero %d a padre\n", i);
            exit(4);
        }
    }
    /*pipe create con successo*/
    
    int pid;
    int fd;
    int j; //per chiudere le pipe
    char lettodafile;
    char lettodapipe;
    long int index = 0; //si comincia a contare da 0
    int count = 0; //per contare le sostituzioni
    for(i = 0; i < N; i++) {
        /*creazione di figli*/
        if((pid = fork()) < 0) {
            printf("Errore nella creazione del figlio numero %d\n", i);
            exit(5);
        }

        if(pid == 0) {
            /*siamo nel figlio*/

            /*come prima cosa chiudiamo tutte le pipe non utlizzate dal figlio corrente*/
            for(j = 0; j < N; j++) {
                close(pipedfatherson[j][1]);
                close(pipedsonfather[j][0]);

                if(i != j) {
                    close(pipedfatherson[j][0]);
                    close(pipedsonfather[j][1]);
                }
            }

            if((fd = open(argv[i + 1], O_RDWR)) < 0) {
                printf("Errore: impossibile aprire file passato come %d-esimo parametro\n", i + 1);
                exit(-1); //se nel figlio si verifica un errore si esce con codice -1 che verrà interpetato dal padre come errore
            }

            while(read(fd, &lettodafile, sizeof(char))) {
                if(lettodafile == c) {
                    write(pipedsonfather[i][1], &index, sizeof(long int)); //scriviamo al padre l'indice
                    read(pipedfatherson[i][0], &lettodapipe, sizeof(char)); //riceviamo il char mandato dal padre

                    if(lettodapipe != '\n') {
                        lseek(fd, -1L, 1);
                        write(fd, &lettodapipe, sizeof(char));
                        count++;
                    } else {
                        /*non facciamo niente*/
                    }
                }

                index++;
            }

            exit(count);
        }
    }

    /*siamo nel padre*/

    /*come prima cosa chiudiamo tutte le pipe non usate*/
    for(i = 0; i < N; i++) {
        close(pipedfatherson[i][0]);
        close(pipedsonfather[i][1]);
    }

    int finito = 0;
    long int indexricevuto;
    char lettodastdin;
    while(finito == 0) {
        finito = 1;
        for(i = 0; i < N; i++) {
            if(read(pipedsonfather[i][0], &indexricevuto, sizeof(long int)) != 0) {
                finito = 0;
                printf("Il figlio di indice %d ha trovato un occorrenza del carattere %c all'indice %li\n", i, c, indexricevuto);
                printf("Se si vuole sostituire tale carattere con ciò che si legge da stdin immettere tale carattere, altrimenti premere invio\n");
                scanf("%c", &lettodastdin);
                write(pipedfatherson[i][1], &lettodastdin, sizeof(char));

                if(lettodastdin != '\n') {
                    scanf("%c", &lettodastdin); //scartiamo l'invio
                }
            }
        }
    }

    /*aspettiamo tutti i figli*/
    int status;
    int ret;
    for(i = 0; i < N; i++) {
        if((pid = wait(&status)) < 0) {
            printf("Errore: la wait del figlio di indice %d non ha avuto successo\n", i);
            exit(6);
        }

        if((status & 0XFF) != 0) {
            printf("Errore durante l'esecuzione del processo %d\n", pid);
        } else {
            ret = (status >> 8) & 0XFF;
            printf("Figlio di pid %d terminato ritornando %d (se 255 problemi)\n", pid, ret);
        }
    }

    exit(0);
}