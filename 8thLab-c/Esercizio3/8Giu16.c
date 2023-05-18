#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define PERMS 0644
typedef int pipe_t[2];

int main(int argc, sschar* argv[]) {
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
    int pid;
    for(i = 0; i < nfile; i++) {
        if(pid = fork() < 0) {
            printf("Errore nella fork\n");
            exit(3);
        }

        if(pid == 0) {
            /*siamo nel figlio*/
            int j;
            for(j = 0; j < nfile; j++) {
                close(pipedfatherson[j][0]);
                close(pipedsonfather[j][1]);
            }

            char c[255];
            int k = 0;
            while(read(argv[i + 1], &c, sizeof(char)) > 0) {
                if(c[k] == '\n') {
                    write(pipedfatherson[1], strlen(c), sizeof(c));
                    azzerabuffer(c);
                    continue;
                }
                k++;
            }

            /*da finire*/
        }
    }

    /*siamo nel padre*/
    int j;
    for(j = 0; j < nfile; j++) {
        close(pipedfatherson[j][1]);
        close(pipedsonfather[j][0]);
    }

    for(i = 0; i < num; i++) {
        int numrandom;
        read(pipedfatherson[mia_random(nfile)][0], &numrandom, sizeof(int));

        int j;
        for(j = 0; j < nfile; j++) {
            write(pipedsonfather[j][1], &numrandom, sizeof(int));
        }
    }
}

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