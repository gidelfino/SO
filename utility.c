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