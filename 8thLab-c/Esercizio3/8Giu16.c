#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define PERMS 0644
typedef int pipe_t[2];

int mia_random(int n) {
    int casuale;
    casuale = rand() % n;
    return casuale;
}

void azzerabuffer(char* c) {
    int i;
    for(i = 0; i < 255; i++) {
        c[i] = 0;
    }
    return;
}

int main(int argc, char* argv[]) {
    if(argc < 6) {
        printf("Errore: pochi parametri\n");
        exit(1);
    }
    
    int nfile = argc - 2;
    int num = atoi(argv[argc - 1]);
    if(num < 0 || num > 255) {
        printf("Ultimo parametro bleah\n");
        exit(2);
    }

    srand(time(NULL));
    int Fcreato = creat("/tmp/creato", PERMS);

    pipe_t* pipedfatherson;
    pipe_t* pipedsonfather;
    pipedfatherson = malloc(nfile * sizeof(pipe_t));
    pipedsonfather = malloc(nfile * sizeof(pipe_t));

    int i;
    for(i = 0; i < nfile; i++) {
        pipe(pipedfatherson[i]);
        pipe(pipedsonfather[i]);
    }

    int pid;
    for(i = 0; i < nfile; i++) {
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(3);
        }

        if(pid == 0) {
            /*siamo nel figlio*/
            int j;
            for(j = 0; j < nfile; j++) {
                if(j != i) {
                    close(pipedfatherson[j][1]);
                    close(pipedsonfather[j][0]);
                }
                close(pipedfatherson[j][0]);
                close(pipedsonfather[j][1]);
            }

            int fd;
            if((fd = open(argv[i + 1], O_RDONLY)) < 0) {
                printf("Errore nella open per il file di indice %d\n", i + 1);
                exit(-1);
            }

            char c[255];
            int k = 0;
            int righescritte = 0;
            while(read(fd, &c[k], sizeof(char)) > 0) {
                if(c[k] == '\n') {
                    k++;
                    printf("La pipe %d manda %d\n", i, k);
                    write(pipedfatherson[i][1], &k, sizeof(k));
                    int indicericevuto;
                    read(pipedsonfather[i][0], &indicericevuto, sizeof(int));

                    if(indicericevuto < k) {
                        write(Fcreato, &c[indicericevuto], sizeof(char));
                        righescritte++;
                    }

                    azzerabuffer(c);
                    k = 0;
                } else {
                    k++;
                }
            }

            exit(righescritte);
        }
    }

    /*siamo nel padre*/
    int j;
    for(j = 0; j < nfile; j++) {
        close(pipedfatherson[j][1]);
        close(pipedsonfather[j][0]);
    }

    for(i = 0; i < num; i++) {
        int r = mia_random(nfile);

        int valore;
        int giusto;
        for(j = 0; j < nfile; j++) {
            read(pipedfatherson[j][0], &valore, sizeof(valore));
            printf("Il valore letto dalla pipe %d per l'indice %d è %d\n", j, i, valore);
            if(j == r) {
                giusto = valore;
            }
        }

        printf("Il padre per l'indice %d ha letto dalla pipe %d e generato l'indice %d\n", i, r, giusto);

        for(j = 0; j < nfile; j++) {
            write(pipedsonfather[j][1], &giusto, sizeof(int));
        }
    }

    int status;
    for(i = 0; i < nfile; i++) {
        if((pid = wait(&status)) < 0) {
            printf("Errore nella wait per il figlio %d\n", i);
        }

        if((status & 0XFF) < 0) {
            printf("Errore nell'esecuzione del figlio di indice %d\n", i);
        }

        int ret = (status >> 8) & 0XFF;
        printf("Il figlio di pid %d è terminato con valore di ritorno %d\n", pid, ret);
    }
    exit(0);
}