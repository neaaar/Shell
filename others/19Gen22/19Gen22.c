#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

typedef int pipe_t[2];

int main(int argc, char* argv[]) {
    int i; //per l'indice dei figli
    /*controllo sul numero dei parametri*/
    if(argc < 3) {
        printf("Errore: numero minimo di parametri non raggiunto, passare al programma almeno 2 parametri\n");
        exit(1);
    }
    int N = argc - 2; //salviamo il numero di file in una variabile di nome N come specificato nel testo

    /*controllo sull'ultimo parametro*/
    char C = atoi(argv[argc - 1]);
    if(C < 0 || (C % 2) != 1) {
        printf("Errore: l'ultimo parametro deve essere un numero strettamente positivo e dispari\n");
        exit(2);
    }

    /*allochiamo la memoria per le pipe*/
    pipe_t* pipedPrimoSecondo; //il primo processo della coppia scrive, il secondo legge
    pipedPrimoSecondo = malloc(N*sizeof(pipe_t));
    if(pipedPrimoSecondo == NULL) {
        printf("Errore: impossibile allocare la memoria per il vettore di pipe\n");
        exit(3);
    }

    /*creiamo le pipe*/
    for(i = 0; i < N; i++) {
        if(pipe(pipedPrimoSecondo[i]) < 0) {
            printf("Errore: impossibile creare la pipe di indice %d\n", i);
        }
    }

    /*abbiamo creato le pipe, possiamo ora passare alla creazione dei figli*/
    int pid;
    int j;
    int nro = 0;
    int pos = 0;
    int fd;
    int fdcreato;
    int lunghezza;
    char* b;
    b = malloc(C * sizeof(char));
    for(i = 0; i < 2*N; i++) {
        if((pid = fork()) < 0) {
            printf("Errore: impossibile creare il processo figlio di indice %d\n", i);
            exit(4);
        }

        if(pid == 0) {
            /*siamo nel figlio*/

            if(i < N) {
                /*siamo nel primo figlio della coppia*/

                /*chiudiamo le pipe che non usa*/
                for(j = 0; j < N; j++) {
                    close(pipedPrimoSecondo[j][0]); //il primo figlio non deve mai leggere dalle pipe
                    if(j != i) close(pipedPrimoSecondo[j][1]); //il primo figlio deve scrivere solo sulla pipe associata alla sua coppia
                }

                /*apriamo il file*/
                if((fd = open(argv[(i % N) + 1], O_RDONLY)) < 0) {
                    printf("Errore: impossibile aprire il file di nome %s nel figlio di indice %d\n", argv[(i % N) + 1], i);
                    exit(-1);
                }
                /*posizioniamo il puntatore sulla posizione giusta all'interno del file*/
                lunghezza = lseek(fd, 0L, SEEK_END); //calcoliamo la lunghezza del file
                lseek(fd, 0, 0); //torniamo all'inizio del file

                while(read(fd, b, C*sizeof(char)) && pos < (lunghezza / 2)) {
                    nro++;
                    pos += C;

                    write(pipedPrimoSecondo[i][1], b, C*sizeof(char));
                }

                exit(nro);
            } else {
                /*siamo nel secondo figlio della coppia*/

                /*chiudiamo le pipe che non usa*/
                for(j = 0; j < N; j++) {
                    close(pipedPrimoSecondo[j][1]); //il secondo figlio non deve mai scrivere sulle pipe
                    if(j != (i % N)) close(pipedPrimoSecondo[j][0]); //il secondo figlio deve leggere solo dalla pipe associata alla sua coppia
                }

                /*creiamo il nuovo file*/
                char* nuovonome;
                nuovonome = malloc((strlen(argv[(i % N) + 1]) + strlen(".mescolato") + 1)*sizeof(char));
                strcat(nuovonome, argv[(i % N) + 1]);
                strcat(nuovonome, ".mescolato");
                if((fdcreato = creat(nuovonome, 0644)) < 0) {
                    printf("Errore: impossibile creare il file di nome %s\n", nuovonome);
                    exit(-1); //usciamo con -1 che verrà interpretato come errore dal padre
                }

                /*apriamo il file*/
                if((fd = open(argv[(i % N) + 1], O_RDONLY)) < 0) {
                    printf("Errore: impossibile aprire il file di nome %s nel figlio di indice %d\n", argv[(i % N) + 1], i);
                }
                /*posizioniamo il puntatore sulla posizione giusta all'interno del file*/
                lunghezza = lseek(fd, 0L, SEEK_END); //calcoliamo la lunghezza del file
                lseek(fd, (long)(lunghezza / 2), SEEK_SET); //ci posizioniamo a metà file

                while(read(fd, b, C*sizeof(char)) != 0) {
                    nro++;

                    write(fdcreato, b, C*sizeof(char)); //scrive sul file
                    read(pipedPrimoSecondo[i % N][0], b, C*sizeof(char)); //legge dalla pipe il blocco mandato dal primo processo della coppia
                    write(fdcreato, b, C*sizeof(char)); //scrive sul file anche quel blocco
                }

                exit(nro);
            }
        }
    }

    /*siamo nel padre*/

    /*chiudiamo tutte le pipe*/
    for(i = 0; i < N; i++) {
        close(pipedPrimoSecondo[i][0]);
        close(pipedPrimoSecondo[i][1]);
    }

    /*aspettiamo tutti i figli*/
    int status, ret;
    for(i = 0; i < 2*N; i++) {
        if((pid = wait(&status)) < 0) {
            printf("Errore nella wait per il figlio di indice %d\n", i);
            exit(5);
        }

        if((status & 0XFF) != 0) {
            printf("Errore: figlio di indice %d terminato in modo anomalo\n", i);
        } else {
            ret = (status >> 8) & 0XFF;
            printf("Figlio di pid %d e indice %i terminato con valore di ritorno %d\n", pid, i, ret);
        }
    }

    exit(0);
}