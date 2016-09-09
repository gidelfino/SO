#define _GNU_SOURCE 	/* Utilizado por sched.h */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>	
#include <sched.h>
#include "utility.h"
#include "minheap.h"
#include "maxheap.h"

#define QUANTUM     1
#define TIME_TOL	0.1 /* Tolerancia de tempo */

void nextProcess(int id)
{
	switch (sched) {
		case 1:	/* First-Come First-Served */
			pthread_mutex_lock(&gmutex);
			pnext = pnext + 1;
			pthread_mutex_unlock(&gmutex);
			
			pthread_mutex_lock(&hmutex);
			threadResume(pnext);
			pthread_mutex_unlock(&hmutex);
			break;

		case 2: /* Shortest Remaining Time Next */
			pthread_mutex_lock(&hmutex);
			maxHeapRemove(running, procs, id);
			pthread_mutex_unlock(&hmutex);
			break;

		case 3: /* Multiplas Filas */
			break;

		default:
			printf("Erro na seleção do próximo processo.\n");
			exit(EXIT_FAILURE);
	}
	pthread_mutex_lock(&gmutex);
	pline++;
	tnumb--;
	pthread_mutex_unlock(&gmutex);
}

void *timeOperation(void *tid)
{
	int i, id = *((int *) tid);
	double elapsed, inactive;
	clock_t start, end;

	if (sched == 1 && tnumb < pnumb)
		threadResume(id);

	/* Se os processadores estao ocupados, esperar */
	threadStatus(id);
	procs[id].cpu = sched_getcpu();

	if (sched == 1) {
		if (dflag == TRUE)
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
		procs[id].cpu = sched_getcpu();
		
		end = clock();
		elapsed = ((double)end - (double)start) / CLOCKS_PER_SEC;
		elapsed -= inactive;
		start = end;

		procs[id].rt -= elapsed;
		procs[id].cq -= elapsed;
		
		/* Tempo de execucao finalizado */
		if (procs[id].rt <= 0.0) { 
			procs[id].tf = ((double)end - (double)gstart) / CLOCKS_PER_SEC;
			break;
		}

		/*Quantum atual finalizado */
		else if (sched == 3 && procs[id].cq <= 0.0) {
			if (dflag == TRUE)
				fprintf(stderr, "Processo da linha [%d] deixou de usar a CPU [%d].\n", 
					procs[id].tl, procs[id].cpu);

			threadPause(id);
			procs[id].cp = (procs[id].cp + 1) % MAX_QUEUE; /* Caso a prioridade for muito alta, */
			procs[id].cq = QUANTUM;						   /* temos que reseta-la para 0 */
			for (i = 0; i < procs[id].cp; i++)
				procs[id].cq *= 2;

			pthread_mutex_lock(&hmutex);
			queue[procs[id].cp][last[procs[id].cp]++] = id;
			pthread_mutex_unlock(&hmutex);

			if (topq > procs[id].cp) topq = procs[id].cp;
			if (last[topq] <= first[topq]) topq = procs[id].cp;
			
			pthread_mutex_lock(&gmutex);
			tnumb--;
			ctxch++;
			pthread_mutex_unlock(&gmutex);
		}

	}		

	if (dflag == TRUE) {
		fprintf(stderr, "Processo da linha [%d]", procs[id].tl);  
		fprintf(stderr, " finalizado, escrito na linha [%d].\n", procs[id].tl);
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
			if (dflag == TRUE) 
				fprintf(stderr, "Processo da linha [%d] chegou ao sistema.\n", procs[i].tl);

			threadCreate(i);
			i++;
		}

	}
}

void shortestRemaining(int n)
{
	int i = 0;
	int topid, botid;
	clock_t end, elapsed;

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
			if (dflag == TRUE) 
				fprintf(stderr, "Processo da linha [%d] chegou ao sistema.\n", procs[i].tl);		
			
			pthread_mutex_lock(&hmutex);
			minHeapInsert(ready, procs, i);
			pthread_mutex_unlock(&hmutex);
			
			threadCreate(i);
			i++;
		}

		/* Processador disponivel e lista de espera nao vazia */
		if (tnumb < pnumb && botid != -1) {
			if (dflag == TRUE)
				fprintf(stderr, "Processo da linha [%d] começou a usar a CPU [%d].\n", 
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

		/* Preempcao quando um dos processos em espera e menor que um executando */
		else if (topid != -1 && botid != -1 && procs[topid].rt > procs[botid].rt) {
			if (dflag == TRUE) {
				fprintf(stderr, "Processo da linha [%d] deixou de usar a CPU [%d].\n", 
					procs[topid].tl, procs[topid].cpu);
				fprintf(stderr, "Processo da linha [%d] começou a usar a CPU [%d].\n", 
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
	}

	pthread_mutex_lock(&hmutex);
	minHeapFree(ready);
	maxHeapFree(running);
	pthread_mutex_unlock(&hmutex);
}

void multiplasFilas(int n) 
{
	int i , j;
	clock_t end, elapsed;
	
	i = topq = 0;

	for (j = 0; j < MAX_QUEUE; j++)
		first[j] = last[j] = 0;

	while (pline < n) {
		end = clock();
		elapsed = ((double)end - (double)gstart) / CLOCKS_PER_SEC;
		
		/* Chegamos ao instante de chegada de uma thread */
		if (elapsed >= procs[i].at - TIME_TOL && elapsed <= procs[i].at + TIME_TOL) {
			if (dflag == 1) 
				fprintf(stderr, "Processo da linha [%d] chegou ao sistema.\n", procs[i].tl);	
			
			pthread_mutex_lock(&hmutex);
			queue[0][last[0]++] = i; 		/* Todos processos comecam na fila de menor quantum */
			pthread_mutex_unlock(&hmutex);

			topq = 0;				/* Primeira fila nao vazia e a de prioridade 0 */
			procs[i].cp = 0;    	/* Prioridade inicial igual a 0 */
			procs[i].cq = QUANTUM;	/* Quantum inicial igual a QUANTUM segundo(s) */
			threadCreate(i);
			i++;
		}

		/* Processadores disponiveis e lista de espera nao vazia */
		if (tnumb < pnumb && last[topq] > first[topq]) {
			if (dflag == 1)
				fprintf(stderr, "Processo da linha [%d] começou a usar a CPU [%d].\n", 
					procs[queue[topq][first[topq]]].tl, procs[queue[topq][first[topq]]].cpu);	
			
			pthread_mutex_lock(&gmutex);
			tnumb++; 			
			pthread_mutex_unlock(&gmutex);

			threadResume(queue[topq][first[topq]]);
			first[topq]++;

			for (j = 0; j < MAX_QUEUE && last[j] <= first[j]; j++); /* Procurando primeira fila nao vazia */
			topq = j;
			if (j == MAX_QUEUE) topq = 0;
		}
	}	
}
