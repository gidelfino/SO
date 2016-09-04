#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>	

#include "utility.h"
#include "scheduler.h"
#include "minheap.h"
#include "maxheap.h"

int dflag;
int pline;
int sched;
int pnext;
int pnumb;
int tnumb;

MinHeap ready;
MaxHeap running;

clock_t gstart;
pthread_mutex_t gmutex;
pthread_mutex_t hmutex;
pthread_mutex_t lmutex;
Process procs[MAX_SIZE];


void swap(int *a, int *b)
{
    int aux = *a;
    *a = *b;
    *b = aux;
}

void nextProcess(int id)
{
	switch (sched) {
		case 1:	/* First-Come First-Served */
			pthread_mutex_lock(&gmutex);
			pnext = pnext + 1;
			threadResume(pnext);
			pthread_mutex_unlock(&gmutex);
			break;
		case 2: /* Shortest Remaining Time Next */
			pthread_mutex_lock(&hmutex);
			maxHeapRemove(running, procs, id);
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

void readFile(char *fname, int *n, Process procs[]) 
{
	char input[MAX_SIZE];

	FILE *file = fopen(fname, "r");
	if (file != NULL)
		while (fgets(input, sizeof(input), file) != NULL) {
			sscanf(strtok(input, " "), "%lf", &procs[*n].at);
			procs[*n].name = malloc(MAX_SIZE * sizeof(char));
			strcpy(procs[*n].name, strtok(NULL, " "));
			sscanf(strtok(NULL, " "), "%lf", &procs[*n].dt);
			sscanf(strtok(NULL, " "), "%lf", &procs[*n].dl);
			procs[*n].rt = procs[*n].dt;
			procs[*n].tl = *n + 1;
			*n = *n + 1;
		}
	else {
		perror("fopen()");
		exit(EXIT_FAILURE);
	}

	fclose(file);
}

void writeFile(char *fname, int n, Process procs[], int ctxch)
{
	int i;

	FILE *file = fopen(fname, "w");
	if (file != NULL) {
		for (i = 0; i < n; i++) {
			fprintf(file, "%s %lf ", procs[i].name, procs[i].tf);
			fprintf(file, "%lf\n", procs[i].tf - procs[i].at);
		}
		fprintf(file, "%d", ctxch);
	}
	else {
		perror("fopen()");
		exit(EXIT_FAILURE);
	}

	fclose(file);
}

void threadCreate(int id)
{
	if (pthread_create(&procs[id].thread, NULL, timeOperation, (void *)&procs[id].id)) {
		printf("Erro ao criar uma thread.\n"); 
		exit(EXIT_FAILURE);
	}
}

void threadPause(int id)
{
	pthread_mutex_lock(&lmutex);
	procs[id].paused = TRUE;
	pthread_mutex_unlock(&lmutex);
}

void threadResume(int id)
{
	pthread_mutex_lock(&lmutex);
	procs[id].paused = FALSE;
	pthread_cond_broadcast(&procs[id].cond);
	pthread_mutex_unlock(&lmutex);
}

void threadStatus(int id)
{
	pthread_mutex_lock(&lmutex);
	while (procs[id].paused)  
		pthread_cond_wait(&procs[id].cond, &lmutex);
	pthread_mutex_unlock(&lmutex);	
}

double threadStatusTime(int id)
{
	clock_t start, end;
	pthread_mutex_lock(&lmutex);
	start = end = clock(); 
	while (procs[id].paused) {  
		end = clock();
		pthread_cond_wait(&procs[id].cond, &lmutex);
	}
	pthread_mutex_unlock(&lmutex);
	return ((double)end - (double)start) / CLOCKS_PER_SEC;	
}