#include <stdio.h>
#include <stdlib.h> 
#include "utility.h"

int HEAPcomp(int a, int b, Process *procs) {

}

int* HEAPinit(int N) {
	int *heap = malloc(100 * N * sizeof(int));
	return heap;
}

void swap(int *a, int *b) {
	int x = *a;
	*a = *b;
	*b = x;
}

static HEAPupdate(int *heap, Process *procs, int no) {
	int pai, x;
	if (no == 1) return;
	pai = no / 2;
	if (HEAPcomp(heap[pai], heap[no], procs)) {
		swap(&heap[pai], &heap[no]);
		HEAPupdate(heap, procs, pai);
	}
}

void HEAPinsert(int *heap, Process *procs, int id, int *sz) {
	*sz++;
	heap[sz] = id;
	HEAPupdate(heap, procs, sz);
}

void HEAPset(int *heap, Process *procs, int no, int sz) {
	int son1, son2;
	if (no * 2 >= sz) return;
	son1 = no * 2; son2 = no * 2 + 1;
	if (HEAPcomp(heap[son1], heap[son2], procs) && HEAPcomp(heap[no], heap[son2], procs)) {
		swap(&heap[no], &heap[son2]);
		HEAPset(son2);
	}
	else if (HEAPcomp(heap[no], heap[son1], procs)) {
		swap(&heap[no], &heap[son1]);
		HEAPset(son1);
	}
}

int HEAPpop(int *heap, int sz) {
	if (sz == 0) return -1;
	int ret = heap[1];
	heap[1] = heap[sz];
	heap[sz] = 10e8;
	sz--; HEAPset(1);
	return ret;
}

/*
PODE NAO PRECISAR PQ O QUE ESTÁ SAINDO PQ FINALIZOU É O MENOR DA FILA
void HEAPremove(int *heap, int id, int sz) {
	int i;
	for (i = 1; i < sz && heap[i] != id; i++);
	heap[i] = 10e8;
	HEAPset(1);
}
*/

//FALAR QUE EU PRECISO EXECUTAR ISSO E NAO USAR O REMOVER
//VOU USAR HEAP DE MAX E MIN PARA A FILA EM EXECUCAO
//PRECISO DO MIN PARA TIRAR O PROCESSO QUE ACABOU EM LOG
