#include "utility.h"

#define CORNER -10e8

struct heap_min {
	int sz;   /* Tamanho da heap */
	int *msk; /* msk[id] = i me dá onde está o processo id dentro da heap (heap[i] = id) */
	int *heap;/* Vetor heap */
};

typedef struct heap_min *HeapMin;

/* Função recebe o número de processadores N e retorna um vetor heap
   já malocado */
HeapMin HEAPinit(int N);

/* A função insere o processo id na heap usando comparando
   com seu tempo restante rt */
void HEAPinsert(HeapMin H, Process procs[], int id) ;

/* A funcao retorna o id do processo com menor tempo restante
   que está sendo processado e o remove da heap */
int HEAPpop(HeapMin H, Process procs[]);

/* A funcao retorna o id do processo com menor tempo restante
   mas não o remove da heap (usar esse para consutas) */
int HEAPtop(HeapMin H);

/* A funcao remove da heap o processo id */
void HEAPremove(HeapMin H, Process procs[], int id);
