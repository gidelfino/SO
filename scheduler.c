#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>	

#include "utility.h"

#define TIME_TOL	0.1 /* Tolerancia de tempo */


void *timeOperation(void *tid)
{
	int id;
	double elapsed;
	clock_t start, end;
	
	id = *((int *) tid);

	/* Se os processadores estao ocupados, esperar */
	if (tnumb >= pnumb)
		threadPause(id);
	threadStatus(id);
	
	pthread_mutex_lock(&gmutex);
	tnumb++; 
	pthread_mutex_unlock(&gmutex);

	/* Simulacao de execucao */
	start = clock();
	while (TRUE) {
		threadStatus(id);
		end = clock();
		elapsed = ((double)end - (double)start) / CLOCKS_PER_SEC;
		start = end;
		procs[id].rt -= elapsed;
		if (procs[id].rt <= 0.0) { 
			procs[id].tf = ((double)end - (double)gstart) / CLOCKS_PER_SEC;
			break; 
		}
	}		

	pthread_mutex_lock(&gmutex);
	tnumb--; 
	pthread_mutex_unlock(&gmutex);

	if (dflag == 1) {
		printf("Processo da linha [%d]", procs[id].tl);  
		printf(" finalizado, escrito na linha [%d].\n", id + 1);
	}
	nextProcess();
	threadResume(pnext);

	return 0;
}

void firstCome(int n)
{
	int i = 0;
	clock_t end, elapsed;

	pnext = pnumb - 1; /* pnumb processos poderao rodar no inicio */

	/* Execucao das threads (ordenadas por ordem de chegada) */
	while (i < n) {
		end = clock();
		elapsed = ((double)end - (double)gstart) / CLOCKS_PER_SEC;

		/* Chegamos ao instante de chegada de uma thread */
		if (elapsed >= procs[i].at - TIME_TOL && elapsed <= procs[i].at + TIME_TOL) {
			if (dflag == 1) 
				printf("Processo da linha [%d] chegou ao sistema.\n", procs[i].tl);
			
			if (pthread_create(&procs[i].thread, NULL, timeOperation, (void *)&procs[i].id)) {
				printf("Erro ao criar uma thread.\n"); 
				exit(EXIT_FAILURE);
			}

			i++;
		}
	}
}
