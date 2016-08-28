#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>	

#include "utility.h"
#include "scheduler.h"

int dflag;
int sched;
int pnext;
int pnumb;
int tnumb;

int allbusy;

clock_t gstart;
pthread_mutex_t gmutex;
Process procs[MAX_SIZE];


void swap(int *a, int *b)
{
    int aux = *a;
    *a = *b;
    *b = aux;
}

void nextProcess() {
	switch (sched) {
		case 1:	/* First-Come First-Served */
			pnext = pnext + 1;
			break;
		default:
			printf("Erro na seleção do próximo processo.\n");
			exit(EXIT_FAILURE);
	}
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

void mutexLock(pthread_mutex_t mutex)
{
	if (pthread_mutex_lock(&mutex) != 0) {
		perror("pthread_mutex_lock()");
		exit(EXIT_FAILURE);
	}
}

void mutexUnlock(pthread_mutex_t mutex)
{	
	if (pthread_mutex_unlock(&mutex) != 0) {
		perror("pthread_mutex_unlock()");
		exit(EXIT_FAILURE);
	}
}

void threadPause(int id)
{
	mutexLock(gmutex);
	procs[id].paused = TRUE;
	mutexUnlock(gmutex);
}

void threadResume(int id)
{
	mutexLock(gmutex);
	procs[id].paused = FALSE;
	pthread_cond_broadcast(&procs[id].cond);
	mutexUnlock(gmutex);
}

void threadStatus(int id)
{
	mutexLock(gmutex);	
	while (procs[id].paused)  
		pthread_cond_wait(&procs[id].cond, &gmutex);
	mutexUnlock(gmutex);

}

/*
if (id == 1) {
		if (pthread_mutex_trylock(&gmutex) == 0) {
			printf("mutex sucedido\n");
			pthread_mutex_unlock(&gmutex);
		}
		else printf("thread %d nao controla mutex\n", id);
	}
*/