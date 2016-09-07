#define _GNU_SOURCE 	/* Utilizado por sched.h */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>	
#include <sched.h>
#include "utility.h"
#include "minheap.h"
#include "maxheap.h"

#define TIME_TOL	0.1 /* Tolerancia de tempo */

int fila[MAX_SIZE];
int ini, fim;

void nextProcess(int id)
{
	switch (sched) {
		case 1:	/* First-Come First-Served */
			pthread_mutex_lock(&gmutex);
			pnext = pnext + 1;
			threadResume(pnext);
			pline++;
			pthread_mutex_unlock(&gmutex);
			break;
		case 2: /* Shortest Remaining Time Next */
			pthread_mutex_lock(&hmutex);
			maxHeapRemove(running, procs, id);
			pline++;
			pthread_mutex_unlock(&hmutex);

			break;
		case 3: /* Multiplas Filas */
			pthread_mutex_lock(&hmutex);
			/* Caso nao tenha mais tempo para rodar nao faco nada */
			if (procs[id].left <= 0.0 + TIME_TOL) {
				pline++; 
				tnumb--;
				return;
			}
			/* Aqui sabemos que o processo passara para a fila seguinte */
			procs[id].qt *= 2.0;
			procs[id].rt = procs[id].left;
			procs[id].left = procs[id].left - procs[id].qt;
			if (procs[id].left > 0.0 - TIME_TOL)
				procs[id].rt = procs[id].qt;
			printf("Colocando processo %d na fila com rt %f e faltando %f\n", procs[id].tl, procs[id].rt, procs[id].left);
			fila[fim++] = id;
			threadPause(id);
			threadCreate(id);
			pthread_mutex_unlock(&hmutex);
			break;
		default:
			printf("Erro na seleção do próximo processo.\n");
			exit(EXIT_FAILURE);
	}
	pthread_mutex_lock(&gmutex);
	tnumb--;
	pthread_mutex_unlock(&gmutex);
}

void *timeOperation(void *tid)
{
	int id;
	double elapsed, inactive;
	clock_t start, end;

	id = *((int *) tid);

	/* Se os processadores estao ocupados, esperar */
	if (sched == 1 && tnumb < pnumb)
		threadResume(id);
	threadStatus(id);
	/* procs[id].cpu = sched_getcpu(); */

	if (sched == 1) {
		printf("Processo da linha [%d] esta usando a CPU [%d]\n", 
			procs[id].tl, procs[id].cpu);
		pthread_mutex_lock(&gmutex);
		tnumb++; 
		pthread_mutex_unlock(&gmutex);
	}

	/* Simulacao de execucao */
	start = clock();
	while (TRUE) {
		inactive = threadStatusTime(id);
		/* procs[id].cpu = sched_getcpu(); */
		end = clock();
		elapsed = ((double)end - (double)start) / CLOCKS_PER_SEC;
		elapsed -= inactive;
		start = end;
		procs[id].rt -= elapsed;
		if (procs[id].rt <= 0.0) { 
			procs[id].tf = ((double)end - (double)gstart) / CLOCKS_PER_SEC;
			break; 
		}
	}		

	if (dflag == 1) {
		printf("Processo da linha [%d]", procs[id].tl);  
		printf(" finalizado, escrito na linha [%d].\n", procs[id].tl);
	};
	
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

void multiplasFilas(int n) 
{
	clock_t end, elapsed;
	int i;

	elapsed = ini = fim = i = 0;
	while (pline < n) {
		end = clock();
		elapsed = ((double)end - (double)gstart) / CLOCKS_PER_SEC;
		/* Chegamos ao instante de chegada de uma thread */
		if (elapsed >= procs[i].at - TIME_TOL && elapsed <= procs[i].at + TIME_TOL) {
			if (dflag == 1) 
				printf("Processo da linha [%d] chegou ao sistema.\n", procs[i].tl);	
			pthread_mutex_lock(&hmutex);
			fila[fim++] = i;
			procs[i].left = procs[i].rt - QUANTUM;
			if (procs[i].left >= 0.0)
				procs[i].rt = QUANTUM;
			procs[i].qt = 1.0;
			pthread_mutex_unlock(&hmutex);
			threadCreate(i);
			i++;
		}

		/* Processadores vazios e lista de espera nao vazia */
		if (tnumb < pnumb && fim > ini) {
			if (dflag == 1)
				printf("Processo da linha [%d] começou a usar a CPU [%d].\n", 
					procs[fila[ini]].tl, procs[fila[ini]].cpu);	
			pthread_mutex_lock(&gmutex);
			tnumb++; 			
			pthread_mutex_unlock(&gmutex);

			threadResume(fila[ini]);
			pthread_mutex_lock(&gmutex);
			ini++; 			
			pthread_mutex_unlock(&gmutex);
		}
	}	
}

void shortestRemaining(int n)
{
	int i = 0;
	int topid, botid;
	clock_t end, elapsed;
	
	elapsed = 0;

	pthread_mutex_lock(&hmutex);
	ready = minHeapInit(n);
	running = maxHeapInit(pnumb);
	pthread_mutex_unlock(&hmutex);

	/* Execucao das threads (ordenadas por ordem de chegada) */
	while (pline < n) {
		end = clock();
		elapsed = ((double)end - (double)gstart) / CLOCKS_PER_SEC;

		pthread_mutex_lock(&hmutex);
		botid = minHeapTop(ready);
		topid = maxHeapTop(running);
		pthread_mutex_unlock(&hmutex);

		/* Chegamos ao instante de chegada de uma thread */
		if (elapsed >= procs[i].at - TIME_TOL && elapsed <= procs[i].at + TIME_TOL) {
			if (dflag == 1) 
				printf("Processo da linha [%d] chegou ao sistema.\n", procs[i].tl);		
			
			pthread_mutex_lock(&hmutex);
			minHeapInsert(ready, procs, i);
			pthread_mutex_unlock(&hmutex);
			
			threadCreate(i);
			i++;
		}

		/* Preempcao quando um dos processos em espera e menor que um executando */
		if (topid != -1 && botid != -1 && procs[topid].rt > procs[botid].rt) {
			if (dflag == 1) {
				printf("Processo da linha [%d] deixou de usar a CPU [%d].\n", 
					procs[topid].tl, procs[topid].cpu);
				printf("Processo da linha [%d] começou a usar a CPU [%d].\n", 
					procs[botid].tl, procs[botid].cpu);
			}

			threadPause(topid);

			pthread_mutex_lock(&hmutex);
			minHeapPop(ready, procs);
			maxHeapInsert(running, procs, botid);
			maxHeapPop(running, procs);
			minHeapInsert(ready, procs, topid);
			pthread_mutex_unlock(&hmutex);

			pthread_mutex_lock(&gmutex);
			ctxch++;
			pthread_mutex_unlock(&gmutex);

			threadResume(botid);
		}

		/* Processador disponivel e lista de espera nao vazia */
		else if (tnumb < pnumb && botid != -1) {
			if (dflag == 1)
				printf("Processo da linha [%d] começou a usar a CPU [%d].\n", 
					procs[botid].tl, procs[botid].cpu);	
			
			pthread_mutex_lock(&gmutex);
			tnumb++; 
			pthread_mutex_unlock(&gmutex);

			pthread_mutex_lock(&hmutex);
			minHeapPop(ready, procs);
			maxHeapInsert(running, procs, botid);
			pthread_mutex_unlock(&hmutex);
			
			threadResume(botid);
		}
	}

	pthread_mutex_lock(&hmutex);
	minHeapFree(ready);
	maxHeapFree(running);
	pthread_mutex_unlock(&hmutex);
}

