#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Errore: numero parametri sbagliato\n");
        exit(1);
    }

    int fd1;
    if((fd1 = open(argv[1], O_RDONLY)) < 0) {
        printf("Errore: impossibile aprire il file\n");
        exit(2);
    }

    char fd2;
    if((fd2 = argv[2][1]) != 0) {
        printf("Errore: parametro 2 non singolo carattere\n");
        exit(3);
    }

    long int occ = 0;
    int ret;
    int c = 0;
    while((ret = read(fd1, &c, 1)) > 0) {
            if(c == argv[2][0]) {
                occ++;
            }
    }

    exit(0);
}