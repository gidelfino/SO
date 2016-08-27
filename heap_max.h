#include "utility.h"

#define CORNER -10e8

struct heap_max {
	int sz;   /* Tamanho da heap */
	int *msk; /* msk[id] = i me dá onde está o processo id dentro da heap (heap[i] = id) */
	int *heap;/* Vetor heap */
};

typedef struct heap_max *MaxHeap;

/* Função recebe o número de processadores N e retorna um vetor heap
   já malocado */
MaxHeap MAXHEAPinit(int N);

/* A função insere o processo id na heap usando que ele 
   começou a ser processado em ct */
void MAXHEAPinsert(MaxHeap H, Process procs[], double ct, int id) ;

/* A funcao retorna o id do processo com maior tempo restante
   que está sendo processado e o remove da heap 
   (ct é o tempo atual) */
int MAXHEAPpop(MaxHeap H, Process procs[], double ct);

/* A funcao retorna o id do processo com maior tempo restante
   mas não o remove da heap (usar esse para consutas) */
int MAXHEAPtop(MaxHeap H);

/* A funcao remove da heap o processo id */
void MAXHEAPremove(MaxHeap H, Process procs[], double ct, int id);

/* ct É SEMPRE O TEMPO ATUAL */
