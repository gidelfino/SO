#ifndef maxheap_H_
#define maxheap_H_

#include "utility.h"

#define CORNER -10e8


/* Struct de uma heap de maximo */
struct heap_max {
	int size;   /* Tamanho da heap */
	int *msk; 	/* msk[id] = i me dá onde está o processo id dentro da heap (heap[i] = id) */
	int *heap;	/* Vetor heap */
};
typedef struct heap_max *MaxHeap;

/* Funcao que recebe o numero N de processadores e retorna 
um vetor heap ja alocado */
MaxHeap MAXHEAPinit(int N);

/* Funcao que insere o processo id na heap sendo ct o tempo que ele 
comecou a ser processado */
void MAXHEAPinsert(MaxHeap H, Process procs[], double ct, int id) ;

/* Funcao que retorna o id do processo com maior tempo restante
que esta sendo processado e o remove da heap (ct e o tempo atual) */
int MAXHEAPpop(MaxHeap H, Process procs[], double ct);

/* Funcao que retorna o id do processo com maior tempo restante
mas nao o remove da heap (utilizado para consutas) */
int MAXHEAPtop(MaxHeap H);

/* Funcao que remove da heap o processo com identificador id */
void MAXHEAPremove(MaxHeap H, Process procs[], double ct, int id);

/* ct É SEMPRE O TEMPO ATUAL */

#endif