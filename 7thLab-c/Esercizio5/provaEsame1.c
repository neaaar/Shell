#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    int i;
    int nfile = argc -1;
    
    int piped[2][2];
    /*piped[0] per i figli di indice pari*/
    /*piped[1] per i i figli di indice dispari*/
    pipe(piped[0]);
    pipe(piped[1]);

    if(argc < 3) {
        printf("Errore: numero parametri sbagliato");
        exit(1);
    }

    int fd;
    for(i = 0; i < nfile; i++) {
        if((fd = open(argv[i + 1], O_RDONLY)) < 0) {
            printf("Errore nell'apertura del file di parametro numero %d", i + 1);
            exit(2);
        }
    }

    int pid;
    for(i = 0; i < nfile; i++) {
        if((pid = fork()) < 0) {
            printf("Errore nella generazione del figlio numero %d", i);
            exit(3);
        }

        if(pid == 0) {
            /*siamo nel figlio*/
            if((i % 2) == 0) {
                /*siamo nel figlio di indice pari*/
                /*associato al file di indice dispari*/

                close(piped[0][0]);
                close(piped[1][0]); /*non serve*/
                close(piped[0][0]); /*chiudiamo il lato di lettura*/

                char c;
                char lastc;
                
                fd = open(argv[i + 1], O_RDONLY);
                if(fd < 0) {
                    printf("Errore nella open\n");
                    exit(7);
                }
                while(read(fd, &c, sizeof(char)) > 0) {
                    if((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) {
                        /*abbiamo trovato un carattere alfabetico*/
                        write(piped[0][1], &c, sizeof(char));
                        /*lo scriviamo sulla pipe*/
                        lastc = c;
                    }
                }

                exit(lastc);
            } else {
                /*siamo nel figlio di indice dispari*/
                /*associato al file di indice pari*/

                close(piped[0][0]);
                close(piped[1][0]); /*non serve*/
                close(piped[1][0]); /*chiudiamo il lato di lettura*/

                char c;
                int lastc;

                fd = open(argv[i + 1], O_RDONLY);
                if(fd < 0) {
                    printf("Errore nella open\n");
                    exit(8);
                }
                while(read(fd, &c, sizeof(char)) > 0) {
                    if(c >= 47 && c <= 57) {
                        /*abbiamo trovato un carattere numerico*/
                        write(piped[0][1], &c, sizeof(char));
                        /*lo scriviamo sulla pipe*/
                        lastc = c;
                    }
                }

                exit(lastc);
            }
        }
    }

    /*siamo nel padre*/
    close(piped[0][1]);
    close(piped[1][1]);
    /*chiudiamo i lati di scrittura delle pipe*/
    int tot = 0;
    char ch0;
    char ch1;
    int nr0 = read(piped[0][0], &ch0, 1);
  	int nr1 = read(piped[1][0], &ch1, 1);

  	while ((nr0 != 0) || (nr1 != 0)) {
    		tot = tot + nr0 + nr1; /* il totale viene ricavato sommando via via i valori di ritorno delle read da pipe, ricordando che uno dei due puo' anche essere 0 nel qual caso una delle due write seguenti NON avra' effetto */
    		write(1, &ch1, nr1); /* ATTENZIONE ALL'ORDINE: prima un alfabetico e poi un numerico (secondo la specifica) */
    		write(1, &ch0, nr0);
    		nr0 = read(piped[0][0], &ch0, 1);
    		nr1 = read(piped[1][0], &ch1, 1);
  	}

    int status;
    for(i = 0; i < nfile; i++) {
        if((pid = wait(&status)) < 0) {
            printf("Errore nella wait per il figlio di indice %d\n", i);
            exit(4);
        }

        if(status & 0XFF) {
            printf("Errore nel figlio di indice %d\n", i);
            exit(5);
        }

        int ret = (status >> 8) & 0XFF;
        printf("Terminato figlio con pid %d, ha ritornato %d\n", pid, ret);
    }

    /*una volta aspettati tutti i figli*/
    printf("Numero totale di caratteri stampati dai figli: %d\n", tot);

    exit(0);
}