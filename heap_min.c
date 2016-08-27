#include <stdio.h>
#include <stdlib.h> 
#include "utility.h"
#include "heap_min.h"

static int HEAPcomp(int a, int b, Process *procs, int debug) {
    if (debug) printf("%s resta %f e %s resta %f\n", procs[a].name, procs[a].rt, procs[b].name, procs[b].rt);
    if (procs[a].rt > procs[b].rt) return 1;
    return 0;
}

HeapMin HEAPinit(int N) {
    HeapMin H; int i;
    H = malloc(sizeof *H);
    H->heap = malloc(2 * N * sizeof(int));
    H->msk = malloc((N + 2) * sizeof(int));
    for (i = 0; i <= N; i++) H->msk[i] = -1;
    H->sz = 0;
    return H;
}

static HEAPupdate(int *heap, Process procs[], int *msk, int no) {
    int pai, x;
    if (no == 1) return;
    pai = no / 2;
    if (HEAPcomp(heap[pai], heap[no], procs, 0)) {
        msk[heap[pai]] = no; msk[heap[no]] = pai;
        swap(&heap[pai], &heap[no]);
        HEAPupdate(heap, procs, msk, pai);
    }
}

void HEAPinsert(HeapMin H, Process procs[], int id) {
    (H->sz)++;
    H->heap[H->sz] = id; H->msk[id] = H->sz;
    HEAPupdate(H->heap, procs, H->msk, H->sz);
}

static  void HEAPset(int heap[], Process procs[], int *msk, int no, int sz) {
    int son1, son2;
    if (no * 2 > sz) return;
    son1 = no * 2; son2 = no * 2 + 1;
    if (son1 == sz) son2 = -1;
    if (son2 != -1 && HEAPcomp(heap[son1], heap[son2], procs, 0) && HEAPcomp(heap[no], heap[son2], procs,0)) {
        msk[heap[no]] = son2; msk[heap[son2]] = no;
        swap(&heap[no], &heap[son2]);
        HEAPset(heap, procs, msk, son2, sz);
    }
    else if (HEAPcomp(heap[no], heap[son1], procs, 0)) {
        msk[heap[no]] = son1; msk[heap[son1]] = no;
        swap(&heap[no], &heap[son1]);
        HEAPset(heap, procs, msk, son1, sz);
    }
}

int HEAPpop(HeapMin H, Process procs[]) {
    if (H->sz == 0) return -1;
    int ret = H->heap[1];
    H->heap[1] = H->heap[H->sz];
    H->heap[H->sz] = -CORNER; (H->sz)--; 
    HEAPset(H->heap, procs, H->msk, 1, H->sz);
    H->msk[ret] = -1;
    return ret;
}

int HEAPtop(HeapMin H) {
    if (H->sz == 0) return -1;
    return H->heap[1];
}

void HEAPremove(HeapMin H, Process procs[], int id) {
    procs[H->heap[H->msk[id]]].pt = CORNER;
    HEAPupdate(H->heap, procs, H->msk, H->msk[id]);
    HEAPpop(H, procs);
}

//Talvez depois possa apagar os msks da Heap de Min, acho que nao precisa usar a funcao remove nessa
