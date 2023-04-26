#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


int main(int argc, char* argv[]) {
    if(argc != 4) {
            printf("Errore: numero parametri sbagliato\n");
            exit(1);
    }

    int fd1;
    if((fd1 = open(argv[1], O_RDONLY)) < 0) {
        printf("Errore: non è stato possibile aprire il primo parametro\n");
        exit(2);
    }

    int fd2;
    if((fd2 = atoi(argv[2])) <= 0) {
        printf("Errore: secondo parametro non numerico o non positivo\n");
        exit(3);
    }

    char fd3;
    if((fd3 = argv[3][1]) != 0) {
        printf("Errore: terzo parametro non singolo carattere\n");
        exit(4);
    }

    printf("%s\n", argv[0]);
    printf("%d\n", argc);
    printf("Nome del file: %s\n", argv[1]);
    printf("Numero: %d\n", fd2);
    printf("Singolo carattere: %c\n", argv[3][0]);

    exit(0);
}