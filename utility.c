#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>	
#include <pthread.h>
#include <time.h>

#include "utility.h"

int dflag;
int sched;
int pnext;
int pnumb;
int tnumb;
clock_t gstart;
Process procs[MAX_SIZE];


/* Funcao que troca os valores de duas variaveis */
void swap(int *a, int *b)
{
    int x = *a;
    *a = *b;
    *b = x;
}

/* Funcao de leitura do arquivo de trace, onde: 
fname o nome do arquivo de trace, n o contador de numero de processos
e procs o  vetor de processos */
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
			procs[*n].rt = procs[*n].dt; //No inicio o rt e dt sao iguais
			procs[*n].tl = *n + 1;
			*n = *n + 1;
		}
	else {
		perror("fopen()");
		exit(EXIT_FAILURE);
	}

	fclose(file);
}

/* Funcao de escrita do arquivo de saida, onde:
fname e o nome do arquivo de saida, n o numero de processos simulados,
procs o vetor de processos e ctxch o numero de mudancas de contexto */
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

/* Funcao que tranca o mutex da thread com identificador id */
void mutexLock(int id)
{
	if (pthread_mutex_lock(&procs[id].mutex) != 0) {
		perror("pthread_mutex_lock()");
		exit(EXIT_FAILURE);
	}
}

/* Funcao que destranca o mutex da thread com identificador id */
void mutexUnlock(int id)
{
	if (pthread_mutex_unlock(&procs[id].mutex) != 0) {
		perror("pthread_mutex_unlock()");
		exit(EXIT_FAILURE);
	}
}

/* Funcao que realiza uma operacao que consome tempo 
real da CPU para o processo de identificador tid */
void *timeOperation(void *tid)
{
	double time, elapsed;
	clock_t start, end;
	int id = *((int *) tid);
	
	time = procs[id].dt;

	if (tnumb < pnumb) 
		ThreadResume(id);
	ThreadStatus(id);
	
	mutexLock(id); 
	nthreads++; 
	if (option == 2) 
		remove_heap(id);
	mutexUnlock(id);

	start = clock();
	while (1) {
		ThreadStatus(id);
		end = clock();
		elapsed = ((double)end - (double)start) / CLOCKS_PER_SEC;
		start = end;
		procs[id].rtime -= elapsed;
		if (procs[id].rtime <= 0.0) { 
			procs[id].timef = ((double)end - (double)gstart) / CLOCKS_PER_SEC;
			break; 
		}

	}		

	mutexLock(id); 
	nthreads--; 
	mutexUnlock(id);
	
	calcNext();
	if (next != -1) 
		ThreadResume(next);
	if (dflag == 1)
		printf("Processo da linha [%d] finalizado, escrito na linha [%d].\n", procs[id].traceline, id + 1);
	
	return NULL;
}
