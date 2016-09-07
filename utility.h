#ifndef utility_H_
#define utility_H_

#include <pthread.h>
#include <time.h>

#define TRUE        1
#define FALSE       0
#define MAX_SIZE    1025
#define QUANTUM     1.0


/* Struct de um processo */
struct process 
{
    double at;      /* Instante (em segundos) de chegada */
    char *name;     /* Nome identificador do processo */
    double dt;      /* Tempo (da CPU) que deve ser simulado */
    double dl;      /* (Deadline) Intante limite de termino */

    int id;         /* Posicao do processo por ordem de chegada */
    int tl;         /* Linha do processo no arquivo de trace */
    int cpu;        /* CPU sendo usada pelo processo */
    int qt;         /* Nivel de quantum separado para esse processo */
    double rt;      /* Tempo restante de execucao */
    double tf;      /* Instante de termino */
    double left;    /* Tempo rt subtraido do quantum */

    int paused;  /* Flag que indica se a thread esta pausada */

    pthread_t thread;       /* Identificador da thread do processo */
    pthread_mutex_t mutex;  /* Mutex associado a thread */
    pthread_cond_t cond;    /* Variavel de condicao da thread */
};
typedef struct process Process;


/* Variaveis globais */
extern int dflag;   /* Parametro d opcional */
extern int pline;   /* Linha de impressao do arquivo de saida */
extern int sched;   /* Escalonador escolhido */
extern int pnext;   /* Proximo processo que esta em espera */
extern int pnumb;   /* Numero de processadores na maquina */
extern int tnumb;   /* Numero de threads executando no momento */
extern int ctxch;   /* Numero de mudancas de contexto */
extern int ini, fim;/* Inicio e fim da fila do escalonador 3 */
extern int fila[MAX_SIZE]; /* Fila do escalonador 3 */

extern clock_t gstart;          /* Instante de inicio do programa */
extern pthread_mutex_t gmutex;  /* Mutex global para uso geral */
extern pthread_mutex_t hmutex;  /* Mutex global para heaps */
extern pthread_mutex_t lmutex;  /* Mutex global para locks e unlocks */

extern Process procs[MAX_SIZE]; /* Vetor de processos */


/* Funcao que troca os valores de duas variaveis */
void swap(int *a, int *b);

/* Funcao de leitura do arquivo de trace, onde: 
fname o nome do arquivo de trace, n o contador de numero de processos
e procs o  vetor de processos */
void readFile(char *fname, int *n, Process procs[]);

/* Funcao de escrita do arquivo de saida, onde:
fname e o nome do arquivo de saida, n o numero de processos simulados,
procs o vetor de processos e ctxch o numero de mudancas de contexto */
void writeFile(char *fname, int n, Process procs[], int ctxch);

/* Funcao que libera a memoria alocada para os processos */
void freeProcess(int n, Process procs[]);

/* Funcao que cria uma thread com identificado id */
void threadCreate(int id);

/* Funcao que pausa a thread de identificador id */
void threadPause(int id);

/* Funcao que pausa a thread de identificador id */
void threadResume(int id);

/* Funcao que verifica se a thread esta pausada. 
Se sim, ela trava a thread ate ela ser resumida */
void threadStatus(int id);

/* Versao do threadStatus que retorna o tempo em que
a thread ficou pausada */
double threadStatusTime(int id);

#endif