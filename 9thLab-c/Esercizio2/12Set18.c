#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

typedef int pipe_t[2];

int main(int argc, char* argv[]) {
    int i;

    /*controllo sul numero di parametri*/
    if(argc < 3) {
        printf("Errore: invocare l'eseguibile includendo almeno 2 parametri");
        exit(1);
    }
    int N = argc - 1;

    pipe_t* piped;
    piped = malloc(N*sizeof(pipe_t));
    for(i = 0; i < N; i++) {
        if(pipe(piped[i]) < 0) {
            printf("Errore nella creazione della pipe %d-esima\n", i);
            exit(2);
        }
    }

    int pid;
    int j;
    int fd;
    char c;
    int offset = 'a' - 'A';
    long int contafiglio = 0;
    long int contanipote = 0;
    int status;
    int ret;
    for(i = 0; i < N; i++) {
        if((pid = fork()) < 0) {
            printf("Errore nella creazione della fork %d-esima\n", i);
            exit(3);
        }

        if(pid == 0) {
            /*siamo nel figlio*/

            /*chiudiamo le pipe*/
            for(j = 0; j < N; j++) {
                close(piped[j][0]);
                if(j != i) close(piped[j][1]);
            }

            if((pid = fork()) < 0) {
                printf("Errore nella generazione del nipote per il figlio di indice %d\n", i);
                exit(-1);
            }

            if(pid == 0) {
                /*siamo nel nipote*/

                if((fd = open(argv[i + 1], O_RDWR)) < 0) {
                    printf("Errore nella open del file passato come parametro numero %d\n", i + 1);
                    exit(-1);
                }

                while(read(fd, &c, sizeof(char))) {
                    if(c >= 'a' && c <= 'z') {
                        contanipote++;
                        lseek(fd, -1L, 1);
                        c -= offset;
                        write(fd, &c, sizeof(char));
                    }
                }

                char n = 'n';
                write(piped[i][1], &n, sizeof(char));
                write(piped[i][1], &contanipote, sizeof(long int));
                
                exit(contanipote / 256);
            }

            if((fd = open(argv[i + 1], O_RDWR)) < 0) {
                printf("Errore nella open del file passato come parametro numero %d\n", i + 1);
                exit(-1);
            }

            while(read(fd, &c, sizeof(char))) {
                if(c >= '1' && c <= '9') {
                    contafiglio++;
                    lseek(fd, -1L, 1);
                    c = ' ';
                    write(fd, &c, sizeof(char));
                }
            }

            char f = 'f';
            write(piped[i][1], &f, sizeof(char));    
            write(piped[i][1], &contafiglio, sizeof(long int));

            if ((pid = wait(&status)) < 0) {
                printf("Errore nella wait\n");
                exit(-1);
            }

            if((status & 0XFF) != 0) {
                printf("Errore durante l'esecuzione del nipote %d\n", i);
            } else {
                ret = (status >> 8) & 0XFF;
                printf("Il nipote di pid %d, collegata al figlio %d, ha ritornato %d\n", pid, i, ret);
            }

            exit(contafiglio / 256);
        }
    }

    /*siamo nel padre*/

    /*chiusura delle pipe*/
    for(i = 0; i < N; i++) {
        close(piped[i][1]);
    }

    long int lettofiglio;
    long int lettonipote;
    for(i = 0; i < N; i++) {
        for(j = 0; j < 2; j++) {
            read(piped[i][0], &c, sizeof(char));
            if(c == 'f') {
                read(piped[i][0], &lettofiglio, sizeof(long int));
            } else if(c == 'n') {
                read(piped[i][0], &lettonipote, sizeof(long int));
            }
        }

        printf("Per il file %s sono stati modificati %li caratteri dal figlio e %li caratteri dal nipote\n", argv[i + 1], lettofiglio, lettonipote);
    }

    for(i = 0; i < N; i++) {
        if ((pid = wait(&status)) < 0) {
            printf("Errore nella wait\n");
            exit(-1);
        }

        if((status & 0XFF )!= 0) {
            printf("Errore durante l'esecuzione del figlio %d\n", i);
        } else {
            ret = (status >> 8) & 0XFF;
            printf("Il figlio di pid %d ha ritornato %d\n", pid, ret);
        }
    }


    exit(0);
}