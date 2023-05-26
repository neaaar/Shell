#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>

typedef int pipe_t[2];

struct struttdati {
    char v1; //per contenere il carattere
    long int v2; //per contenere il numero di occorrenze di v1
};

void scambia(struct struttdati* v1, struct struttdati* v2) {
    struct struttdati tmp = *v1;
    *v1 = *v2;
    *v2 = tmp;
}

void bubbleSort(struct struttdati v[], int dim) { 
    int i; bool ordinato = false;
    while (dim>1 && !ordinato) { 
        ordinato = true; /* hp: è ordinato */
        for (i=0; i<dim-1; i++)
        if (v[i].v2>v[i+1].v2) {
            scambia(&v[i],&v[i+1]);
            ordinato = false;
            }
        dim--;
    }
} 

int main(int argc, char* argv[]) {
    /*controllo sul numero di parametri*/
    if(argc != 2) { //controllo stretto, 1 solo parametro
        printf("Errore: inserire un solo parametro");
        exit(1);
    }

    /*definizione dell'array di pipe*/
    pipe_t piped[26];
    int i;
    for(i = 0; i < 26; i++) {
        if(pipe(piped[i]) < 0) {
            printf("Errore nella creazione della %i-esima pipe", i);
            exit(2);
        }
    }  

    /*definizione dell'array di struct*/
    struct struttdati strutturadati[26];

    /*abbiamo tutte le pipe, possiamo procedere alla creazione dei figli*/
    char c = 'a'; //variabile usata per associare ogni figlio ad una lettera
    int pid[26];
    int fd;
    char letto;
    int j;
    struct struttdati structtemp;
    for(i = 0; i < 26; i++) {
        int pidFiglio;
        if((pidFiglio = fork()) < 0) {
            printf("Errore nella creazione di un processo figlio\n");
            exit(3);
        }

        if(pidFiglio == 0) {
            /*siamo nel figlio*/
            for(j = 0; j < 26; j++) {
                if(j != i - 1 || i == 0) {
                    close(piped[j][0]);
                }
                if(j != i) {
                    close(piped[j][1]);
                }
            }

            structtemp.v1 = c + i;

            fd = open(argv[1], O_RDONLY);
            while(read(fd, &letto, sizeof(char)) != 0) {
                if(letto == structtemp.v1) {
                    structtemp.v2++;
                }
            }

            if(i != 0) {
                read(piped[i - 1][0], strutturadati, sizeof(struct struttdati) * 26);
                strutturadati[i] = structtemp;
            }

            write(piped[i][1], strutturadati, sizeof(strutturadati));
            exit(letto);
        }
        pid[i] = pidFiglio;
    }

    /*siamo nel padre*/
    for(i = 0; i < 26; i++) {
        close(piped[i][1]);
        if(i != 25) {
            close(piped[i][0]);
        }
    }

    read(piped[25][0], strutturadati, sizeof(strutturadati));
    bubbleSort(strutturadati, 26);

    for(i = 0; i < 26; i++) {
        printf("Il figlio di pid %d ha generato la struttura dati di indice %d\n", pid[strutturadati[i].v1 - 'a'], i);
    }

    int status;
    for(i = 0; i < 26; i++) {
        if((pid[i] = wait(&status)) < 0) {
            printf("Errore nella wait\n");
            exit(4);
        }

        if((status & 0XFF) != 0) {
            printf("Errore nello status del figlio %d\n", pid[i]);
        } else {
            char ret = (status >> 8) &0XFF;
            printf("Il figlio di pid %d ha ritornato %c equivalente a %d in ascii\n", pid[i], ret, ret);
        }
    }

    exit(0);
}