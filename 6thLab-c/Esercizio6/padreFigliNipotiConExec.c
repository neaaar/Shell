#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#define PERMS 0644

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
            strcpy(file, argv[i + 1]);
            strcat(file, ".sort");
            int fd = creat(file, PERMS);

            if(fd < 0) {
                printf("Errore nella creazione del file\n");
                exit(3);
            }
            close(fd);

            int pidnip = fork();
            if(pidnip < 0) {
                printf("Errore nella fork di un nipote");
                exit(4);
            }

            if(pidnip == 0) {
                close(0);
                if((open(argv[i + 1], O_RDONLY)) < 0) {
                    printf("Errore nell'apertura di %s", argv[i + 1]);
                }
                close(1);
                if((open(file, O_WRONLY)) < 0) {
                    printf("Errore nell'apertura di %s", file);
                }

                execlp("sort", "sort", (char *)0);
                perror("Errore nel comando sort\n");
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