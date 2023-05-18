#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>

typedef int pipe_t[2];

int main(int argc, char* argv[]) {
    int status;

    if(argc < 3) {
        printf("Errore: pochi parametri\n");
        exit(1);
    }

    pipe_t *piped;
    piped = malloc((argc - 1)*sizeof(pipe_t));
    
    int i;
    for(i = 0; i < argc -1; i++) {
        if(pipe(piped[i]) < 0) {
            printf("Errore nella creazione della pipe\n");
            exit(2);
        }
    }

    for(i = 0; i < argc - 1; i++) {
        int pid;
        if((pid = fork()) < 0) {
            printf("Errore nella fork per il figlio numero %i\n", i);
            exit(3);
        }

        if(pid == 0) {
            /*siamo nel figlio*/
            int j, pidnip;
            for(j = 0; j < argc - 1; j++) {
                close(piped[j][0]);
            }

            int pipedf[2];
            pipe(pipedf);

            char c[255];
            close(0);
            if((open(argv[i + 1], O_RDONLY)) < 0) {
                printf("Errore nella open di un file\n");
                exit(4);
            }

            if((pidnip = fork()) < 0) {
                printf("Errore nella generazione di un nipote\n");
                exit(5);
            }

            if(pidnip == 0) {
                /*siamo nel nipote*/
                close(pipedf[0]);
                close(1);
                dup(pipedf[1]);
                close(pipedf[1]);
                printf("Exec per il file %s", argv[i + 1]);
                execlp("wc", "wc", "-l", (char *)0);
                printf("Errore nella exec del comando wc\n");
                exit(6);
            }

            if(wait(&status) < 0) {
                printf("Errore mentre si aspettava un nipote\n");
                exit(-1);
            }

            close(pipedf[1]);
            int k = 0;
            while(read(pipedf[0], &c[k], sizeof(char)) > 0) {
                if(c[k] == '\n') {
                    c[k] = 0;
                    break;
                }
                k++;
            }

            int num = atoi(c);
            write(piped[i][1], &num, sizeof(int));

            exit((status >> 8) & 0XFF);
        }
    }

    /*siamo nel padre*/
    for(i = 0; i < argc - 1; i++) {
        close(piped[i][1]);
    }

    long int totlines = 0;
    for(i = 0; i < argc - 1; i++) {
        int num;
        read(piped[i][0], &num, sizeof(int));
        totlines += num;    
    }

    for(i = 0; i < argc - 1; i++) {
        int pid;
        if((pid = wait(&status)) < 0) {
            printf("Errore mentre si aspettava un figlio\n");
        }

        int ret = (status >> 8) & 0XFF;
        printf("Filgio con pid %d concluso con codice %d\n", pid, ret);
    }

    printf("Totale linee: %li\n", totlines);
    exit(0);
}