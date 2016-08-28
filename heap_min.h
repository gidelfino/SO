#ifndef minheap_H_
#define minheap_H_

#include "utility.h"

#define CORNER -10e8


/* Struct de uma heap de minimo */
struct heap_min {
	int size;   /* Tamanho da heap */
	int *msk; 	/* msk[id] = i me dá onde está o processo id dentro da heap (heap[i] = id) */
	int *heap;	/* Vetor heap */
};
typedef struct heap_min *MinHeap;

/* Funcao que recebe o numero de processadores N e retorna
um vetor heap ja alocado */
MinHeap MINHEAPinit(int N);

/* Funcao que insere o processo id na heap utilizando
como comparacao seu tempo restante rt */
void MINHEAPinsert(MinHeap H, Process procs[], int id) ;

/* Funcao que retorna o id do processo com menor tempo restante
que esta sendo processado e o remove da heap */
int MINHEAPpop(MinHeap H, Process procs[]);

/* Funcao que retorna o id do processo com menor tempo restante
mas nao o remove da heap (utilizado para consutas) */
int MINHEAPtop(MinHeap H);

/* Funcao que remove da heap o processo id */
void MINHEAPremove(MinHeap H, Process procs[], int id);

#endif