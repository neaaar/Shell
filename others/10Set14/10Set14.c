#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

typedef int pipe_t[2];

typedef struct {
    int lunghezza;
    int indice;
}strutt;

void bubbleSort(strutt v[], int dim) {
    int i;
    int ordinato = 0;
    while (dim>1 && !ordinato) {
        ordinato = 1; /* hp: è ordinato */ 
        for (i=0; i<dim-1; i++) {
            if (v[i].lunghezza>v[i+1].lunghezza) { 
                strutt tmp = v[i];
                v[i] = v[i + 1];
                v[i + 1] = tmp;
                ordinato = 0;
            } 
        }
        dim--;
    } 
}

int main(int argc, char* argv[]) {
    int i;

    /*controllo sul numero dei parametri*/
    if(argc < 3) {
        printf("Errore: numero minimo di parametri non raggiunto, passare al programma almeno 2 parametri\n");
        exit(1);
    }
    int N = argc - 2; //salviamo il numero di file

    /*creiamo il vettore di struct*/
    strutt* vettore;
    vettore = malloc(N*sizeof(strutt));
    if(vettore == NULL) {
        printf("Errore nell'allocazione della memoria per il vettore di struct");
        exit(8);
    }

    int K = atoi(argv[argc - 1]); //salviamo l'ultimo parametro come numero
    if(K <= 0) {
        printf("Errore: l'ultimo parametro deve essere un numero strettamente positivo\n");
        exit(2);
    }

    /*richiesta all'utente di X*/
    int X;
    printf("Inserire un numero strettamente positivo e minore o uguale a K\n");
    scanf("%d", &X);
    if(X < 0 || X > K) {
        printf("Errore: il numero inserito non rispetta le specifiche\n");
        exit(10);
    }

    /*allocazione memoria + creazione delle pipe*/
    pipe_t* pipedPadreFiglio;
    pipedPadreFiglio = malloc(N*sizeof(pipe_t));
    if(pipedPadreFiglio == NULL) {
        printf("Errore nell'allocazione della memoria per le pipe da padre a figlio\n");
        exit(3);
    }

    pipe_t* pipedFiglioPadre;
    pipedFiglioPadre = malloc(N*sizeof(pipe_t));
    if(pipedFiglioPadre == NULL) {
        printf("Errore nell'allocazione della memoria per le pipe da figlio a padre");
        exit(4);
    }

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

    /*possiamo ora passare alla creazione dei figli*/
    int pid;
    int j;
    int fd;
    char linea[512];
    int L = 0;
    int lineacorrente = 1;
    char c;

    for(i = 0; i < N; i++) {
        if((pid = fork()) < 0) {
            printf("Errore nella creazione del figlio di indice %d\n", i);
            exit(7);
        }

        if(pid == 0) {
            /*siamo nel figlio*/

            /*per prima cosa chiudiamo tutte le pipe non utilizzate*/
            for(j = 0; j < N; j++) {
                close(pipedPadreFiglio[j][1]); //chiusura lato scrittura per tutte le pipe padre figlio
                close(pipedFiglioPadre[j][0]); //chiusura lato lettura per tutte le pipe figlio padre

                if(j != i) {
                    close(pipedPadreFiglio[j][0]);
                    close(pipedFiglioPadre[j][1]);
                }
            }

            if((fd = open(argv[i + 1], O_RDONLY)) < 0) {
                printf("Errore nell'apertura del file di nome %s\n", argv[i + 1]);
                exit(-1); //si ritorna -1 che verrà interpretato dal padre come un errore
            }

            while(read(fd, &linea[L], sizeof(char)) != 0) {
                if (linea[L] == '\n') {
                	if (lineacorrente == X){ /* trovata la linea che dobbiamo selezionare */
				        printf("STAMPA DI DEBUGGING: La linea numero %d del file %s e':\n%s", j, argv[i+1], linea);
                    	break; /* usciamo dal ciclo di lettura */
 			        } else {      
                        L = 0; /* azzeriamo l'indice per la prossima linea */
                    	lineacorrente++;    /* se troviamo un terminatore di linea incrementiamo il conteggio */
                	}
        	    }	
        	    else L++;
            }
            L++;

            write(pipedFiglioPadre[i][1], &L, sizeof(int));
            read(pipedPadreFiglio[i][0], &c, sizeof(char));
            printf("%s\n", linea);

            exit(L % 255);
        }
    }

    /*siamo nel padre*/

    /*chiudiamo le pipe non utilizzate*/
    for(i = 0; i < N; i++) {
        close(pipedPadreFiglio[i][0]);
        close(pipedFiglioPadre[i][1]);
    }

    for(i = 0; i < N; i++) {
        read(pipedFiglioPadre[i][0], &vettore[i].lunghezza, sizeof(int));
    }

    bubbleSort(vettore, N);

    c = 'v'; //vai
    for(i = 0; i < N; i++) {
        printf("Si riporta ora su stdout la linea numero %d del file %s\n", X, argv[i + 1]);
        write(pipedPadreFiglio[vettore[i].indice][1], &c, sizeof(char));
        sleep(1);
    }

    /*finito tutto possiamo passare ad aspettare i figli*/
    int status;
    int ret;
    for(i = 0; i < N; i++) {
        if((pid = wait(&status)) < 0) {
            printf("Errore nella wait per il figlio di indice %d\n", i);
            exit(9);
        }

        if((status & 0XFF) != 0) {
            printf("Errore durante l'esecuzione del processo di indice %d\n", i);
        } else {
            ret = (status >> 8) & 0XFF;
            printf("Il processo di pid %d ha ritornato %d (se 255 errore)\n", pid, ret);
        }
    }

    exit(0);
}