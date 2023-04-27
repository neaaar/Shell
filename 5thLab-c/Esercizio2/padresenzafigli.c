#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    int pid = getpid();
    printf("%d\n", pid);

    int status;
    int waitret = wait(&status);

    if(waitret < 0) {
        printf("Errore\n");
        exit(1);
    }

    exit(0);
}