#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

typedef int pipe_t[2];

int main(int argc, char* argv[]) {
    int i; //per scorrere i cicli for

    /*controllo sul numero di parametri*/
    if(argc < 4) {
        printf("Errore: numero minimo di parametri (3) non raggiunto\n");
        exit(1);
    }

    /*nel padre apriamo il file passato come ultimo parametro*/
    int fd;
    if((fd = open(argv[argc - 1], O_RDONLY)) < 0) {
        printf("Errore: impossibile aprire il file passato come ultimo parametro\n");
        exit(2);
    }

    int N = argc - 2;
    /*a questo punto possiamo creare all'allocazione della memoria i vettori piped*/
    pipe_t* pipedPadreFiglio; //pipe con padre scrittore e figli lettori
    pipedPadreFiglio = malloc(N*sizeof(pipe_t));
    if(pipedPadreFiglio == NULL) {
        printf("Errore nell'allocazione della memoria per le pipe da padre a figlio\n");
        exit(3);
    }

    pipe_t* pipedFiglioPadre;
    pipedFiglioPadre = malloc(N*sizeof(pipe_t));
    if(pipedFiglioPadre == NULL) {
        printf("Errore nell'allocazione della memoria per le pipe da figlio a padre\n");
        exit(4);
    }

    /*una volta allocata la memoria possiamo creare le pipe*/
    for(i = 0; i < N; i++) {
        if(pipe(pipedPadreFiglio[i]) < 0) {
            printf("Errore nella creazione della pipe da padre a figlio numero %d\n", i);
            exit(5);
        }

        if(pipe(pipedFiglioPadre[i]) < 0) {
            printf("Errore nella creazione della pipe da figlio a padre numero %d\n", i);
            exit(6);
        }
    }

    /*create tutte le pipe, possiamo ora creare i figli*/
    int* pid; //per salvare i pid dei figli
    pid = malloc(N*sizeof(int));
    if(pid == NULL) {
        printf("Errore nell'allocazione della memoria per il vettore di pid\n");
        exit(7);
    }
    int j; //per chiudere le pipe all'interno dei figli
    char c; //per salvare il carattere letto/il carattere ricevuto dal padre
    for(i = 0; i < N; i++) {
        if((pid[i] = fork()) < 0) {
            printf("Errore nella creazione del figlio di indice %d\n", i);
            exit(8);
        }

        if(pid[i] == 0) {
            /*siamo nel figlio*/

            /*per prima cosa chiudiamo tutte le pipe non utilizzate*/
            for(j = 0; j < N; j++) {
                close(pipedPadreFiglio[j][1]); //non scrive mai su queste pipe
                close(pipedFiglioPadre[j][0]); //non legge mai da queste pipe

                if(j != i) {
                    close(pipedPadreFiglio[j][0]); //se i != j non legge neanche
                    close(pipedFiglioPadre[j][1]); //se i != j non scrive neanche
                }
            }

            /*proviamo ad aprire il file associato*/
            if((fd = open(argv[i + 1], O_RDONLY)) < 0) {
                printf("Errore: impossibile aprire il file di nome %s\n", argv[i + 1]);
                exit(-1); //usciamo con -1 che verrà interpretato dal padre come un errore
            }

            while(1) {
                read(pipedPadreFiglio[i][0], &c, sizeof(char)); //si aspetta che il padre mandi la conferma di procedere
                if(c == 't') break; //se il padre dice di terminare perché il file è finito, allora usciamo dal while e terminiamo il figlio

                read(fd, &c, sizeof(char)); //leggiamo 1 carattere dal file
                write(pipedFiglioPadre[i][1], &c, sizeof(char)); //scriviamo al padre il carattere letto
            }

            printf("DEBUG: Sono il figlio %d di indice %d e ho finito di leggere il file %s\n", getpid(), i, argv[i+1]); 
            exit(0);
        }
    }

    /*siamo nel padre*/

    /*chiudiamo le pipe non utilizzate*/
    for(i = 0; i < N; i++) {
        close(pipedPadreFiglio[i][0]); //il padre non deve mai leggere da queste pipe
        close(pipedFiglioPadre[i][1]); //il padre non deve mai scrivere su queste pipe
    }

    /*creiamo il vettore di int per segnare se il confronto con il carattere mandato da un figlio non ha avuto successo*/
    int*  vettore;
    vettore = malloc(N*sizeof(int));
    if(vettore == NULL) {
        printf("Errore nell'allocazione della memoria per il vettore di int\n");
        exit(9);
    }

    /*se l'allocazione ha avuto successo, lo inizializiamo con una serie di 1*/
    for(i = 0; i < N; i++) {
        vettore[i] = 1;
    }

    /*a questo punto possiamo iniziare a leggere il file passato come ultimo parametro*/
    char cf; //per salvare il carattere letto dall'ultimo file
    char cfiglio = 'v'; //carattere da mandare ai figli, v sta per vai
    while(read(fd, &cf, sizeof(char)) != 0) {
        for(i = 0; i < N; i++) {
            if(vettore[i] == 1) { //scriviamo al figlio solo se il confronto finora ha avuto successo
                write(pipedPadreFiglio[i][1], &cfiglio, sizeof(char));

                read(pipedFiglioPadre[i][0], &c, sizeof(char));
                if(c != cf) {
                    vettore[i] = 0;
                }
            }
        }
    }

    /*una volta finito di leggere il file, scriviamo ai figli con i quali il confronto ha avuto successo di uscire dal while e agli altri di terminare*/
    cfiglio = 't';
    for(i = 0; i < N; i++) {
        if(vettore[i] == 1) {
            printf("Il padre scrive al figlio di indice %d di uscire dal while\n", i);
            write(pipedPadreFiglio[i][1], &cfiglio, sizeof(char)); //si scrive al figlio di terminare
        } else {
            printf("Il padre scrivo al figlio di indice %d di terminare con il segnale SIGKILL\n", i);
            kill(pid[i], SIGKILL);
        }
    }

    /*altro ciclo for per aspettare i figli non terminati con il segnale*/
    int status, ret;
    int pidFiglio;
    for(i = 0; i < N; i++) {
        if((pidFiglio = wait(&status)) < 0) {
            printf("Errore nella wait per il figlio di indice %d\n", i);
            exit(10);
        }

        if((status & 0XFF) != 0) {
            printf("Figlio di indice %d terminato in modo anomalo\n", i);
        } else {
            ret = (status >> 8) & 0XFF;
            printf("Il figlio di pid %d  e indice %d è terminato ritornando %d (se 255 errore)\n", pid[i], i, ret);

            for(j = 0; j < N; j++) {
                if(pidFiglio == pid[j]) {
                    printf("Questo significa che il figlio di indice %d ha verificato che il file %s e' uguale al file %s\n", j, argv[j+1], argv[argc-1]);
                }
            }
        }
    }

    exit(0);
}