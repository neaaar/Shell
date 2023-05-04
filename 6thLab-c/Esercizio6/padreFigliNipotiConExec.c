#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    int status;
    int ret;

    if(argc < 4) {
        printf("Errore nel numero dei parametri");
        exit(1);
    }

    int i;
    for(i = 0; i < (argc - 1); i++) {
        if(open(argv[i + 1], O_RDONLY) < 0) {
            printf("Errore nella open del file %d", i);
        }
    }

    for(i = 0; i < (argc - 1); i++) {
        int pid = fork();
        if(pid < 0) {
            printf("Errore nella fork numero %d", i);
            exit(2);
        }

        if(pid == 0) {
            char *file = malloc(strlen(argv[i + 1]) + strlen(".sort") + 1);
            file = strcat(argv[i + 1], ".sort");
            int fd = creat(file, O_RDWR);

            if(fd < 0) {
                printf("Errore nella creazione del file\n");
                exit(3);
            }

            int pidnip = fork();
            if(pidnip < 0) {
                printf("Errore nella fork di un nipote");
                exit(4);
            }

            if(pidnip == 0) {
                close(0);
                open(argv[i + 1], O_RDONLY);
                close(1);
                open(file, O_RDWR);
                execlp("sort", "sort", (char *)0);
                exit(-1);
            }

            wait(&status);
            ret = (status >> 8) & 0XFF;

            exit(ret);
        }
    }

    for(i = 0; i < (argc - 1); i++) {
        int pidfiglio = wait(&status);
        ret = (status >> 8) & 0XFF;
        printf("Il figlio di pid %d ha ritornato %d\n", pidfiglio, ret);
    }

    exit(0);
}