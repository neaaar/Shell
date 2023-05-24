#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

typedef int pipe_t[2];

/*definizione della struttura dati usata dai figli*/
struct retfiglio {
    int c1; //deve contenere il pid del nipote
    int c2; //deve conrenere la lunghezza della linea (compreso il terminatore)
    char c3[250]; //deve contenere la prima linea del file ordinato dal processo nipote
};

int main(int argc, char* argv[]) {
    /*numero di parametri maggiore o uguale a 3*/
    if(argc < 4) {
        printf("Errore nel numero dei parametri: passare almeno 3 parametri al programma\n");
        exit(1);
    }
    int N = argc - 1; //numero file

    /*creazione delle pipe che verrano usate dai figli per passare la struct al padre*/
    pipe_t* piped;
    piped = malloc(N*sizeof(pipe_t));

    int i; //indice per il ciclo for
    for(i = 0; i < N; i++) {
        if(pipe(piped[i]) < 0) {
            printf("Errore nella creazione della pipe di indice %d\n", i);
            exit(2);
        }
    }

    /*abbiamo tutte le pipe, possiamo passare alla generazione degli N figli*/
    int pid; //per salvare i pid dei figli
    int pidnipote; //per salvare i pid dei nipoti
    int fd; //per salvare i file descriptor dei file aperti nei figli
    int status; //per le wait
    int k = 0; //per contare il numero di caratteri
    int p[2]; //per pipe figli-nipoti
    for(i = 0; i < N; i++) {
        if((pid = fork()) < 0) {
            printf("Errore nella creazione del figlio numero %d\n", i);
            exit(3);
        }

        if(pid == 0) {
            /*ci troviamo nel figlio*/
            /*chiudiamo tutte le pipe che non servono*/
            int j;
            for(j = 0; j < N; j++) {
                close(piped[j][0]); //i figli non devono leggere da nessuna pipe
                if(j != i) {
                    close(piped[j][1]); //chiudiamo anche il lato di scrittura per le pipe non associate al figlio corrente
                }
            }

            /*creiamo la struct che verrà passata al padre tramite la pipe*/
            struct retfiglio ret;

            /*apriamo il file che verrà ordinato dal nipote*/
            if((fd = open(argv[i + 1], O_RDWR)) < 0) {
                printf("Errore nell'apertura del file passato come %d-esimo parametro\n", i + 1);
                exit(-1);
            }

            pipe(p);

            /*creiamo il nipote*/
            if((pidnipote = fork()) < 0) {
                printf("Errore nella creazione del nipote per il figlio di indice %d\n", i);
                exit(-1);
            }

            if(pidnipote == 0) {
                /*siamo nel nipote*/
                close (piped[i][1]); //chiusura pipe del figlio dato che il nipote non la usa
                close (p[0]); //chiusura lato pipe che non usa
                close(1); //chiudiamo stdout per ridirigerlo sulla pipe
                dup(p[1]); //ridirezione stdoutput sulla pipe 
                close(p[1]);
                printf("inizio del nipote del figlio %d\n", i);
                execlp("sort", "sort", argv[i + 1], (char *)0);

                /*se giungiamo fin qua c'è stato un errore nella exec*/
                printf("Errore nella exec del comando sort per il figlio di indice %d\n", i);
                exit(-1);
            }
            close(p[1]);

            if(wait(&status) != pidnipote) {
                printf("Errore nell'esecuzione del nipote associato al figlio di indice %d\n", i);
                exit(-1);
            }

            if((status & 0XFF) != 0) {
                printf("Errore nell'esecuzione del nipote associato al figlio di indice %d\n", i);
                exit(-1);
            }

            while(read(p[0], &ret.c3[k], sizeof(char))) {
                if(ret.c3[k] == '\n') {
                    ret.c1 = pidnipote;
                    ret.c2 = k + 1;

                    write(piped[i][1], &ret, sizeof(struct retfiglio));

                    break;
                }
                k++;
            }

            exit(k);
        }
    }

    /*siamo nel padre*/
    /*chiudiamo i lati di scrittura di tutte le pipe*/
    for(i = 0; i < N; i++) {
        close(piped[i][1]);
    }

    /*leggiamo dalle pipe rispettando l'ordine dei file*/
    struct retfiglio ricevuta;
    for(i = 0; i < N; i++) {
        read(piped[i][0], &ricevuta, sizeof(struct retfiglio));

        ricevuta.c3[ricevuta.c2 - 1] = 0; //inseriamo il terminatore
        printf("Il figlio %d ha generato il nipote di pid %d, che ha passato %d caratteri al padre producendo la seguente stringa:\n", i, ricevuta.c1, ricevuta.c2);
        printf("%s, generata ordinando la prima linea del file %s\n", ricevuta.c3, argv[i + 1]);
    }

    /*for con cui si aspettano i figli*/
    int ret; //per il valore di ritorno dei figli
    for(i = 0; i < N; i++) {
        if((pid = wait(&status)) < 0) {
            printf("Errore nella wait per il figlio di indice %d\n", i);
            exit(4);
            }

        if((status & 0XFF) != 0) {
            printf("Errore durante l'esecuzione per il figlio di indice %d\n", i);
            exit(5);
        }

        ret = (status >> 8) & 0XFF;
        printf("Il figlio di pid %d ha ritornato %d\n", pid, ret);
    }

    exit(0);
}