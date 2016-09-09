#ifndef minheap_H_
#define minheap_H_

#include "utility.h"


/* Struct de uma heap de minimo */
struct minheap {
	int size;   /* Tamanho da heap */
	int *msk; 	/* msk[id] = i me dá onde está o processo id dentro da heap (heap[i] = id) */
	int *heap;	/* Vetor da heap */
};
typedef struct minheap *MinHeap;


/* Variaveis globais */
extern MinHeap ready;	/* Heap de processos em espera */


/* Funcao que recebe o numero de processadores N e retorna
um vetor heap ja alocado */
MinHeap minHeapInit(int n);

/* Funcao que insere o processo id na heap utilizando
como comparacao seu tempo restante rt */
void minHeapInsert(MinHeap H, Process procs[], int id) ;

/* Funcao que retorna o remove o processo com 
menor tempo restante que esta sendo processado */
void minHeapPop(MinHeap H, Process procs[]);

/* Funcao que retorna o id do processo com menor tempo restante
mas nao o remove da heap (utilizado para consutas) */
int minHeapTop(MinHeap H);

/* Funcao que remove da heap o processo id */
void minHeapRemove(MinHeap H, Process procs[], int id);

/* Funcao que libera toda a memoria alocada para a heap */
void minHeapFree(MinHeap H);

#endif