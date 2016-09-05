#ifndef maxheap_H_
#define maxheap_H_

#include "utility.h"


/* Struct de uma heap de maximo */
struct maxheap {
	int size;   /* Tamanho da heap */
	int *msk; 	/* msk[id] = i me da onde esta o processo id dentro da heap (heap[i] = id) */
	int *heap;	/* Vetor heap */
};
typedef struct maxheap *MaxHeap;


/* Variaveis globais */
extern MaxHeap running;		/* Heap de processos em execucao */


/* Funcao que recebe o numero n de processadores 
e retorna um vetor heap ja alocado */
MaxHeap maxHeapInit(int n);

/* Funcao que insere o processo id na heap */
void maxHeapInsert(MaxHeap H, Process procs[], int id) ;

/* Funcao que remove o processo com maior 
tempo restante que esta sendo processado */
void maxHeapPop(MaxHeap H, Process procs[]);

/* Funcao que retorna o id do processo com maior tempo restante
mas nao o remove da heap (utilizado para consutas) */
int maxHeapTop(MaxHeap H);

/* Funcao que remove da heap o processo com identificador id */
void maxHeapRemove(MaxHeap H, Process procs[], int id);

/* Funcao que libera toda a memoria alocada para a heap */
void maxHeapFree(MaxHeap H);

#endif