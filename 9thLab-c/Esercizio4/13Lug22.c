#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

typedef int pipe_t[2];

int main(int argc, char* argv[]) {
    int i; //per scorrere i cicli for

    /*controllo sul numero dei parametri*/
    if(argc < 5) {
        printf("Errore: numero di parametri minimo non raggiunto, passare almeno 3 parametri\n");
        exit(1);
    }

    /*controllo su L*/
    int L = atoi(argv[2]);
    if(L <= 0) {
        printf("Errore: il secondo parametro deve essere strettamente positivo\n");
        exit(2);
    }

    /*controllo su C1, ..., Cn*/
    int Q = argc - 3; //salviamo il numero di singoli caratteri passati
    for(i = 0; i < Q; i++) {
        if(argv[i + 3][1] != 0) {
            printf("Errore: il parametro passato come %d-esimo non è un singolo carattere\n", i + 2);
            exit(3);
        }
    }

    /*a questo punto possiamo creare le pipe*/
    pipe_t* piped;
    piped = malloc((Q + 1)*sizeof(pipe_t));
    if(piped == NULL) {
        printf("Errore: impossibile allocare la memoria per il vettore di pipe\n");
        exit(4);
    }

    for(i = 0; i < Q + 1; i++) {
        if(pipe(piped[i]) < 0) {
            printf("Errore: impossibile creare la pipe di indice %d\n", i);
            exit(5);
        }
    }

    /*create le pipe possiamo creare i figli*/
    int q;
    int pid;
    char c;
    int fd;
    char letto;
    int count = 0;
    int countultimalinealetta;
    for(q = 0; q < Q; q++) {
        if((pid = fork()) < 0) {
            printf("Errore: impossibile creare il figlio di indice %d\n", q);
            exit(5);
        }

        if(pid == 0) {
            /*siamo nel figlio*/

            /*chiudiamo le pipe che non servono al figlio*/
            for(i = 0; i < Q + 1; i++) {
                if(i != q) close(piped[i][0]);
                if(i != q + 1) close(piped[i][1]);
            }

            c = argv[q + 3][0]; //associamo ogni figlio al suo singolo carattere

            if((fd = open(argv[1], O_RDONLY)) < 0) {
                printf("Errore: impossibile aprire il file passato come primo parametro\n");
                exit(-1); //nei figli usciamo con codice -1 che verrà interpretato dal padre come un errore
            }   

            while(read(fd, &letto, sizeof(char)) != 0) {
                if(letto == '\n') {
                    /*siamo arrivati alla fine di una riga, ci mettiamo in attesa*/
                        if(read(piped[q][0], &letto, sizeof(char)) > 0) {
                            printf("figlio con indice %d e pid %d ha letto %d caratteri %c nella linea corrente\n", q, getpid(), count, c);
                            countultimalinealetta = count;
                            count = 0; //rimettiamo count a 0
                            /*scriviamo al prossimo figlio di procedere*/
                            write(piped[q + 1][1], &letto, sizeof(char));
                        }
                }

                if(letto == c) {
                    count++;
                }
            }

            exit(countultimalinealetta);
        }
    }

    /*siamo nel padre*/

    /*chiudiamo le pipe non usate*/
    for(i = 0; i < Q + 1; i++) {
        if(i != Q) close(piped[i][0]);
        if(i != 0) close(piped[i][1]);
    }

    letto = 's'; //start
    for(i = 0; i < L; i++) {
        printf("Linea %d del file %s\n", i + 1, argv[1]);
        write(piped[0][1], &letto, sizeof(char));
        read(piped[Q][0], &letto, sizeof(char));
    }

    /*aspettiamo tutti i figli e controlliamo i loro valori di ritorno*/
    int status;
    int ret;
    for(i = 0; i < Q; i++) {
        if((pid = wait(&status)) < 0) {
            printf("Errore: la wait del figlio di indice %d non è andata a buon fine\n", i);
            exit(6);
        }

        if((status & 0XFF) != 0) {
            printf("Errore durante l'esecuzione del processo di indice %d\n", i);
        } else {
            ret = (status >> 8) & 0XFF;
            printf("Il figlio di pid %d ha ritornato %d (se 255 errore)\n", pid, ret);
        }
    }

    exit(0);
}