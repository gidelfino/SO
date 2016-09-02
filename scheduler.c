#define _GNU_SOURCE 	/* Utilizado por sched.h */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>	
#include <sched.h>

#include "utility.h"
#include "heap_min.h"
#include "heap_max.h"

#define TIME_TOL	0.1 /* Tolerancia de tempo */


void *timeOperation(void *tid) /* COLOCAR OS PRINTS DOS DFLAGS */
{
	int id;
	double elapsed, inactive;
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
		inactive = threadStatusTime(id);	
		end = clock();
		elapsed = ((double)end - (double)start) / CLOCKS_PER_SEC;
		elapsed -= inactive;
		start = end;
		procs[id].rt -= elapsed;
		if (procs[id].rt <= 0.0) { 
			printf("%f ::::::::::::: %f\n", elapsed, inactive);
			procs[id].tf = ((double)end - (double)gstart) / CLOCKS_PER_SEC;
			break; 
		}
	}		

	if (dflag == 1) {
		printf("Processo da linha [%d]", procs[id].tl);  
		printf(" finalizado, escrito na linha [%d].\n", pline + 1);
	}
	pline = pline + 1;

	nextProcess(id);

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

			threadCreate(i);
			i++;
		}
	}
}

void shortestRemaining(int n)
{
	int i = 0;
	int topid, botid;
	double current;
	clock_t end, elapsed;
	
	ready = MINHEAPinit(n);
	running = MAXHEAPinit(pnumb);

	/* Execucao das threads (ordenadas por ordem de chegada) */
	while (i < n) {
		end = clock();
		elapsed = ((double)end - (double)gstart) / CLOCKS_PER_SEC;

		topid = MAXHEAPtop(running);
		botid = MINHEAPtop(ready);
		/* Preempcao quando um dos processos em espera e menor que um executando */
		if (topid != -1 && botid != -1 && procs[topid].rt > procs[botid].rt) {
			printf("Trocando %s por %s\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<", procs[topid].name, procs[botid].name);
			threadPause(topid);
			current = ((double)clock() - (double)gstart) / CLOCKS_PER_SEC;
			pthread_mutex_lock(&hmutex);
			MINHEAPpop(ready, procs);
			MAXHEAPinsert(running, procs, current, botid);
			MAXHEAPpop(running, procs, current);
			MINHEAPinsert(ready, procs, topid);
			pthread_mutex_unlock(&hmutex);
			pthread_mutex_lock(&gmutex);
			tnumb--;
			threadResume(botid);
			pthread_mutex_unlock(&gmutex);			
		}
		/* Processador disponivel e lista de espera nao vazia */
		if (tnumb < pnumb && botid != -1) {
			printf("Processadores sobrando entrou %s\n", procs[botid].name);
			current = ((double)clock() - (double)gstart) / CLOCKS_PER_SEC;
			pthread_mutex_lock(&hmutex);
			printf("Antes do pop sz %d\n", ready->size);
			MINHEAPpop(ready, procs);
			printf("Depois do pop sz %d\n", ready->size);
			MAXHEAPinsert(running, procs, current, botid);
			pthread_mutex_unlock(&hmutex);
			threadResume(botid);
		}
		/* Chegamos ao instante de chegada de uma thread */
		if (elapsed >= procs[i].at - TIME_TOL && elapsed <= procs[i].at + TIME_TOL) {
			if (dflag == 1) 
				printf("Processo da linha [%d] chegou ao sistema.\n", procs[i].tl);		
			MINHEAPinsert(ready, procs, i);
			printf("top da heap %d %d %d size %d\n", ready->heap[1], ready->heap[2], ready->heap[3], ready->size);
			threadCreate(i);
			i++;
		}
	}
}