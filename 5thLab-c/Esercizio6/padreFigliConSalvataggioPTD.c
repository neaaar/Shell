#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Errore: numero parametri sbagliato\n");
        exit(1);
    }

    int num = atoi(argv[1]);
    if(num < 0 || num >= 155) {
        printf("Errore: il parametro deve essere un numero positivo e minore di 155\n");
        exit(2);
    }

    printf("ID del processo corrente: %d, numero passato come parametro: %d\n", getpid(), num);

    int i;
    for(i = 0; i < num; i++) {
        int forkret = fork();
        if(forkret < 0) {
            printf("Errore nella generazione del processo %d\n", i);
            exit(3);
        }

        if(forkret == 0) {
            printf("Stampa del processo figlio numero %d, il cui pid è %d\n", i, getpid());

            srand(time(NULL));
            int rnd = rand() % (101 + i);
            exit(rnd);
        }
    }

    int *pid;
    if ((pid=(int *)malloc(N*sizeof(int))) == NULL)	/* bisogna sempre controllare che la malloc abbia avuto successo! */
	{
        	printf("Errore allocazione pid\n");
        	exit(3);
	}

    for (i=0; i < N; i++)
	{
		if ((pidFiglio=wait(&status)) < 0)
        	{
                	printf("Errore in wait %d\n", pidFiglio);
                	exit(5);
        	}
        	if ((status & 0xFF) != 0)
                	printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        	else
        	{ 
			ritorno=(int)((status >> 8) & 0xFF);
			/* la specifica richiede che il padre, oltre il valore di ritorno e il PID, stampi anche il numero d'ordine e questo puo' essere fatto solo analizzando il contenuto dell'array di pid e riportando l'indice di tale array. ATTENZIONE CHE SAREBBE ASSOLUTAMENTE ERRATO ANDARE A STAMPARE L'INDICE DEL FOR ESTERNO! */
			for (j=0; j < N; j++)
				if (pidFiglio == pid[j])
				{
        				printf("Il figlio con pid=%d di indice %d ha ritornato %d\n", pidFiglio, j, ritorno);	/* attenzione a stampare j e NON i!!! */
					break;
				}	
        	}	 
	}
    
    exit(0);
}