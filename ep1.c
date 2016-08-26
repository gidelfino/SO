#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>	
#include <pthread.h>
#include <time.h>
#include <limits.h>

#define MAX_SIZE	 1025
#define TIME_TOL	 0.1

int nprocs, nthreads, dflag, flags[MAX_SIZE];
int heap[100 * MAX_SIZE], szheap;
int next, option;  
clock_t gstart;

struct process { // Struct de processos
	double time, dtime, rtime, deadline, timef;
	char *name;
	int p, id, flag;
	int traceline;
	pthread_mutex_t mutex;
	pthread_cond_t  cond;
	pthread_t thread;
};

typedef struct process Process;
Process procs[MAX_SIZE]; // Array de processos

void swap(int *a, int *b) { // Funcoes utilizadas pela estrutura de dados heap, utilizada para encontrar
	int aux = *a;			// rapidamente elementos que obedecem certa especificacao (ex: menor dtime)
	*a = *b;
	*b = aux;
}

int compNode(int a, int b) {
	if (procs[heap[a]].dtime > procs[heap[b]].dtime) 
		return 1;
	return 0;
}

void updateHeap(int node) {
	int pai, aux;
	if (node == 1) 
		return;
	pai = node / 2;
	if (compNode(pai, node)) {
		swap(&heap[pai], &heap[node]);
		updateHeap(pai);
	}
}

void pushHeap(int id) {
	szheap++;
	heap[szheap] = id;
	updateHeap(szheap);
}

void setHeap(int node) {
	int son1, son2; 
	if (node * 2 >= szheap) 
		return;
	son1 = node * 2;
	son2 = node * 2 + 1;
	if (compNode(son1,son2) && compNode(node, son2)) {
		swap(&heap[node], &heap[son2]);
		setHeap(son2);
	}
	else if (compNode(node, son1)) {
		swap(&heap[node], &heap[son1]);
		setHeap(son1);
	}
}

int popHeap() {
	if(szheap == 0) 
		return -1;
	int ret = heap[1];
	heap[1] = heap[szheap];
	heap[szheap] = 1024;
	szheap--;
	setHeap(1);
	return ret;
}

void remove_heap(int id) {
	int i;
	for (i = 1; i <= szheap; i++)
		if (heap[i] == id) 
			break;
	heap[i] = 1024;
	setHeap(1);
}

int comp_t(const void *p1, const void *p2) { // Funcao utilizada pelo qsort para ordenacao de processos por tempo de inicio
	const struct process *e1 = p1;
	const struct process *e2 = p2;
	if (e1->time > e2->time) 
		return 1;
	return 0;
}

void readTraceFile(int argc, char *fn, int *n, Process procs[]) { // Funcao de leitura do arquivo de trace
	char input[MAX_SIZE];
	FILE *file = fopen(fn, "r");
	if (file != NULL)
		while (fgets(input, sizeof(input), file) != NULL) {
			sscanf(strtok(input, " "), "%lf", &procs[*n].time);
			procs[*n].name = malloc(MAX_SIZE * sizeof(char));
			strcpy(procs[*n].name, strtok(NULL, " "));
			sscanf(strtok(NULL, " "), "%lf", &procs[*n].dtime);
			sscanf(strtok(NULL, " "), "%lf", &procs[*n].deadline);
			sscanf(strtok(NULL, " "), "%d", &procs[*n].p);
			procs[*n].traceline = *n + 1;
			*n = *n + 1;
		}
	else {
		perror("fopen()");
		exit(EXIT_FAILURE);
	}
	fclose(file);
}

void writeTraceFile(char *fn, int n, Process procs[], int cc) { // Funcao de escrita do arquivo de trace
	int i;
	FILE *file = fopen(fn, "w");
	if (file != NULL) {
		for (i = 0; i < n; i++) 
			fprintf(file, "%s %lf %lf\n", procs[i].name, procs[i].timef, procs[i].timef - procs[i].time);
		fprintf(file, "%d", cc);
	}
	else {
		perror("fopen()");
		exit(EXIT_FAILURE);
	}
	fclose(file);
}

void mutexLock(int id) {
	if (pthread_mutex_lock(&procs[id].mutex) != 0) {
		perror("pthread_mutex_lock()");
		exit(EXIT_FAILURE);
	}
}

void mutexUnlock(int id) {
	if (pthread_mutex_unlock(&procs[id].mutex) != 0) {
		perror("pthread_mutex_unlock()");
		exit(EXIT_FAILURE);
	}
}

void ThreadResume(int id) {
	mutexLock(id);
	procs[id].flag = 1;
	pthread_cond_broadcast(&(procs[id].cond));
	mutexUnlock(id);
}

void ThreadStatus(int id) {
	mutexLock(id);
	while (!procs[id].flag)
		pthread_cond_wait(&(procs[id].cond), &(procs[id].mutex));
	mutexUnlock(id);
}

void calcNext() { // Funcao que calcula qual proximo processo deve ser calculado a partir de uma heap ordenada
	switch (option) {
		case 1:
			next = next + 1;
			break;
		case 2:
			next = popHeap();
			break;
	}
}

void *realTimeOperation(void *tid) { // Funcao que realiza uma operacao que consoome tempo real da CPU
	double time, elapsed;
	int id = *((int *) tid);
	clock_t start, end;
	time = procs[id].dtime;

	if (nthreads < nprocs) 
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

void firstCome(int n) { // First-Come First-Served
	int i, rc, thread_args[MAX_SIZE];
	clock_t end, elapsed;
	
	next = nprocs - 1;
	for (i = 0; i < n; ) {	
		end = clock();
		elapsed = ((double)end - (double)gstart) / CLOCKS_PER_SEC;
		if (elapsed >= procs[i].time - TIME_TOL && elapsed  <= procs[i].time + TIME_TOL) {
			if (dflag == 1)
				printf("Processo da linha [%d] chegou ao sistema.\n", procs[i].traceline);
			rc = pthread_create(&procs[i].thread, NULL, realTimeOperation, (void *) &procs[i].id); 
			assert(0 == rc);
			i++;
		}
	}
}

void shortestJob(int n) { // Shortest Job First
	int i, rc, thread_args[MAX_SIZE];
	clock_t end, elapsed;

	for (i = 0; i < n; ) {	
		end = clock();
		elapsed = ((double)end - (double)gstart) / CLOCKS_PER_SEC;
		if (elapsed >= procs[i].time - TIME_TOL && elapsed  <= procs[i].time + TIME_TOL) {
			if (dflag == 1)
				printf("Processo da linha [%d] chegou ao sistema.\n", procs[i].traceline);
			pushHeap(i);
			rc = pthread_create(&procs[i].thread, NULL, realTimeOperation, (void *) &procs[i].id); 
			assert(0 == rc);
			i++;
		}
	}
}

int main(int argc, char *argv[]) {
 	int i, rc, n = 0;
 	nprocs   = sysconf(_SC_NPROCESSORS_ONLN); // numero de processadores do sistema 
 	gstart   = clock();
	szheap   = 0;
 	nthreads = 0;
 	dflag 	 = 0;
	procs[1024].dtime = INT_MAX;
 	
 	for (i = 0; i < MAX_SIZE; i++) 
 		flags[i] = 1;

 	if (argc >= 4) {   // parametros: 1- numero do escalonador 2- nome do arquivo trace 3- nome do arquivo a ser criado
  		if (argc == 5) //			  4- (opcional) caracter d
  			if (strcmp(argv[4], "d") == 0)
  				dflag = 1;
  		readTraceFile(argc, argv[2], &n, procs);
  		qsort(procs, n, sizeof(Process), comp_t);
		for (i = 0; i < n; i++) { 
			procs[i].id = i;	
			procs[i].flag = 0;
			procs[i].rtime = procs[i].dtime;
		}
		option = *argv[1] - '0';
		switch (*argv[1]) {
			case '1':
				firstCome(n);
				break;
			case '2':
				shortestJob(n);
				break;
			default:
				printf("Escalonador escolhido incorreto ou nao implementado.\n");
				exit(EXIT_FAILURE);
		}
	}
	else {
		printf("Numero incorreto de parametros.\n");
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < n; i++) { //esperando as threads terminarem
		rc = pthread_join(procs[i].thread, NULL);
		assert(0 == rc);
	}
	if (dflag == 1) 
		printf("Numero de mudancas de contexto: %d.\n", 0);
	writeTraceFile(argv[3], n, procs, 0);
	for (i = 0; i < n; i++) {
		free(procs[i].name);
		pthread_mutex_destroy(&procs[i].mutex);
		pthread_cond_destroy(&procs[i].cond);
	}
 	return 0;
}	
