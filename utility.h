#ifndef utility_H_
#define utility_H_

#include <pthread.h>
#include <time.h>

#define MAX_SIZE     1025


/* Struct de um processo */
struct process 
{
    double at;      /* Instante (em segundos) de chegada */
    char *name;     /* Nome identificador do processo */
    double dt;      /* Tempo (da CPU) que deve ser simulado */
    double dl;      /* (Deadline) Intante limite de termino */

    int id;         /* Posicao do processo por ordem de chegada */
    int tl;         /* Linha do processo no arquivo de trace */
    double pt;      /* Instante que a thread comeca a ser processada */
    double rt;      /* Tempo restante de execucao */
    double tf;      /* Instante de termino */

    pthread_t thread;       /* Identificador da thread do processo */
    pthread_mutex_t mutex;  /* Mutex associado a thread */
};
typedef struct process Process;


/* Variaveis globais */
extern int dflag;   /* Parametro d opcional */
extern int sched;   /* Escalonador escolhido */
extern int pnext;   /* Proximo processo a ser executado */
extern int pnumb;   /* Numero de processadores na maquina */
extern int tnumb;   /* Numero de threads executando no momento */

extern clock_t gstart;          /* Instante de inicio do programa */
extern Process procs[MAX_SIZE]; /* Vetor de processos */


/* Funções de utility.c, que contem as funcoes de aspecto utilitario */
void swap(int *a, int *b);
void readFile(char *fname, int *n, Process procs[]);
void writeFile(char *fname, int n, Process procs[], int ctxch);
void mutexLock(int id);
void mutexUnlock(int id);


#endif