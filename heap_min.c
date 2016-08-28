#include <stdio.h>
#include <stdlib.h> 
#include "utility.h"
#include "heap_min.h"

static int MINHEAPcomp(int a, int b, Process *procs, int debug) {
    if (debug) printf("%s resta %f e %s resta %f\n", procs[a].name, procs[a].rt, procs[b].name, procs[b].rt);
    if (procs[a].rt > procs[b].rt) return 1;
    return 0;
}

MinHeap MINHEAPinit(int N) {
    MinHeap H; int i;
    H = malloc(sizeof *H);
    H->heap = malloc(2 * N * sizeof(int));
    H->msk = malloc((N + 2) * sizeof(int));
    for (i = 0; i <= N; i++) H->msk[i] = -1;
    H->size = 0;
    return H;
}

static void MINHEAPupdate(int *heap, Process procs[], int *msk, int no) {
    int pai;
    if (no == 1) return;
    pai = no / 2;
    if (MINHEAPcomp(heap[pai], heap[no], procs, 0)) {
        msk[heap[pai]] = no; msk[heap[no]] = pai;
        swap(&heap[pai], &heap[no]);
        MINHEAPupdate(heap, procs, msk, pai);
    }
}

void MINHEAPinsert(MinHeap H, Process procs[], int id) {
    (H->size)++;
    H->heap[H->size] = id; H->msk[id] = H->size;
    MINHEAPupdate(H->heap, procs, H->msk, H->size);
}

static void MINHEAPset(int heap[], Process procs[], int *msk, int no, int size) {
    int son1, son2;
    if (no * 2 > size) return;
    son1 = no * 2; son2 = no * 2 + 1;
    if (son1 == size) son2 = -1;
    if (son2 != -1 && MINHEAPcomp(heap[son1], heap[son2], procs, 0) && MINHEAPcomp(heap[no], heap[son2], procs,0)) {
        msk[heap[no]] = son2; msk[heap[son2]] = no;
        swap(&heap[no], &heap[son2]);
        MINHEAPset(heap, procs, msk, son2, size);
    }
    else if (MINHEAPcomp(heap[no], heap[son1], procs, 0)) {
        msk[heap[no]] = son1; msk[heap[son1]] = no;
        swap(&heap[no], &heap[son1]);
        MINHEAPset(heap, procs, msk, son1, size);
    }
}

int MINHEAPpop(MinHeap H, Process procs[]) {
    int ret;
    if (H->size == 0) return -1;
    ret = H->heap[1];
    H->heap[1] = H->heap[H->size];
    H->heap[H->size] = -CORNER; (H->size)--; 
    MINHEAPset(H->heap, procs, H->msk, 1, H->size);
    H->msk[ret] = -1;
    return ret;
}

int MINHEAPtop(MinHeap H) {
    if (H->size == 0) return -1;
    return H->heap[1];
}

void MINHEAPremove(MinHeap H, Process procs[], int id) {
    procs[H->heap[H->msk[id]]].pt = CORNER;
    MINHEAPupdate(H->heap, procs, H->msk, H->msk[id]);
    MINHEAPpop(H, procs);
}

/* Talvez depois possa apagar os msks da Heap de Min, acho que nao precisa usar a funcao remove nessa */
