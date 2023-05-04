#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    if(argc != 1) {
        printf("Errore: il programma non ha bisogno di parametri");
        exit(1);
    }

    printf("Inserire 0 se si vuole terminare la chiamata ricorsiva\n");
    int n;
    scanf("%d", &n);

    if(n != 0) {
        printf("Esecuzione ricorsiva\n");
        int ret = execl("/home/marco/Esercizi/6thLab-c/Esercizio1/prova", "prova", (char *)0);
        if(ret == -1) printf("Errore nella exec\n");
    }

    printf("Terminazione del programma\n");
    exit(0);
}