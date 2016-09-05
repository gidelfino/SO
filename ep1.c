#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>	
#include <pthread.h>
#include <time.h>

#include "utility.h"
#include "scheduler.h"


int main(int argc, char *argv[])
{
	int i, n;

	n = dflag = pnext = ctxch = 0;
	pnumb = 1; /* sysconf(_SC_NPROCESSORS_ONLN); */

	/* argv[]:	
	1- No. escalonador	3- Arquivo saida
	2- Arquivo trace	4- (Opcional) caracter d */ 
 	if (argc >= 4) { 
  		if (argc == 5)
  			if (strcmp(argv[4], "d") == 0)
  				dflag = TRUE;

  		readFile(argv[2], &n, procs);
  		/* ORDENAR OS PROCESSOS POR ORDEM DE CHEGADA */

		for (i = 0; i < n; i++) { 
			procs[i].id = i;
			procs[i].rt = procs[i].dt;
			procs[i].paused = TRUE;
		}

		/* Execucao do escalonador escolhido */
		sched = *argv[1] - '0';
		switch (sched) {
			case 1: /* First-Come First-Served */
				printf("First-Come First-Served\n");
				firstCome(n);
				break;
			case 2: /* Shortest Remaining Time Next */
				printf("Shortest Remaining Time Next\n");
				shortestRemaining(n);
				break;
			case 3: /* Escalonamento com multiplas filas */
				printf("Escalonamento com múltiplas filas\n");
				break;
			default:
				printf("Escalonador escolhido incorreto.\n");
				exit(EXIT_FAILURE);
		}

		for (i = 0; i < n; i++)
			if (pthread_join(procs[i].thread, NULL)) {
				printf("Erro ao esperar uma thread finalizar.\n"); 
				exit(EXIT_FAILURE);
			}
		
		writeFile(argv[3], n, procs, ctxch);
		freeProcess(n, procs);

	}
	else {
		printf("Numero incorreto de parametros.\n");
		exit(EXIT_FAILURE);
	}

 	return 0;
}	

/* LEMBRAR DO FREES, PRINTS */