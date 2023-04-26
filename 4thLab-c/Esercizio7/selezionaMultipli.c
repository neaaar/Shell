#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    char buffer[1];
    int cnt = 1;

    if(argc != 3) {
        printf("Errore: numero parametri sbagliato\n");
        exit(1);
    }

    int fd;
    if((fd = open(argv[1], O_RDONLY)) < 0) {
        printf("Errore: non è stato possibile aprire come file il primo parametro\n");
        exit(2);
    }

    int num = atoi(argv[2]);
    if(num <= 0) {
        printf("Errore: il secondo parametro deve essere > 0\n");
        exit(3);
    }

    while(read(fd, buffer, 1) > 0) {
        if((cnt % atoi(argv[2])) == 0) {
            printf("Il carattere multiplo %d-esimo all'interno del file %s è %c\n", cnt, argv[1], buffer[0]);
        }
        cnt++;
    }
    
    exit(0);
}