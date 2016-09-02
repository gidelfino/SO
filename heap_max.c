#include <stdio.h>
#include <stdlib.h> 

#include "utility.h"
#include "heap_max.h"

static int MAXHEAPcomp(int a, int b, Process *procs, double ct, int debug)
{
    double ra, rb;  /* Tempo restante atual dos processos a e b */
    ra = procs[a].dt - (ct - procs[a].pt); 
    rb = procs[b].dt - (ct - procs[b].pt);
    if (debug) printf("(%f) %s resta %f e %s resta %f\n", ct, procs[a].name, ra, procs[b].name, rb);
    if (ra < rb) return 1;
    return 0;
}

MaxHeap MAXHEAPinit(int N)
{
    MaxHeap H; int i;
    H = malloc(sizeof *H);
    H->heap = malloc(2 * N * sizeof(int));
    H->msk = malloc((N + 2) * sizeof(int));
    for (i = 0; i <= N; i++) H->msk[i] = -1;
    H->size = 0;
    return H;
}

static void MAXHEAPupdate(int *heap, Process procs[], int *msk, double ct, int no)
{
    int pai;
    if (no == 1) return;
    pai = no / 2;
    if (MAXHEAPcomp(heap[pai], heap[no], procs, ct, 0)) {
        msk[heap[pai]] = no; msk[heap[no]] = pai;
        swap(&heap[pai], &heap[no]);
        MAXHEAPupdate(heap, procs, msk, ct, pai);
    }
}

void MAXHEAPinsert(MaxHeap H, Process procs[], double ct, int id)
{
    (H->size)++;
    H->heap[H->size] = id; H->msk[id] = H->size;
    procs[id].pt = ct; /* Processo id comecou a ser processado em ct */
    MAXHEAPupdate(H->heap, procs, H->msk, ct, H->size);
}

static void MAXHEAPset(int heap[], Process procs[], int *msk, double ct, int no, int size)
{
    int son1, son2;
    if (no * 2 > size) return;
    son1 = no * 2; son2 = no * 2 + 1;
    if (son1 == size) son2 = -1;
    if (son2 != -1 && MAXHEAPcomp(heap[son1], heap[son2], procs, ct, 0) && MAXHEAPcomp(heap[no], heap[son2], procs, ct, 0)) {
        msk[heap[no]] = son2; msk[heap[son2]] = no;
        swap(&heap[no], &heap[son2]);
        MAXHEAPset(heap, procs, msk, ct, son2, size);
    }
    else if (MAXHEAPcomp(heap[no], heap[son1], procs, ct, 0)) {
        msk[heap[no]] = son1; msk[heap[son1]] = no;
        swap(&heap[no], &heap[son1]);
        MAXHEAPset(heap, procs, msk, ct, son1, size);
    }
}

void MAXHEAPpop(MaxHeap H, Process procs[], double ct)
{
    int ret;
    if (H->size == 0) return;
    ret = H->heap[1];
    H->heap[1] = H->heap[H->size];
    /* H->heap[H->size] = CORNER; */
    (H->size)--; 
    MAXHEAPset(H->heap, procs, H->msk, ct, 1, H->size);
    H->msk[ret] = -1;
    procs[ret].rt -= (ct - procs[ret].pt); /* Atualiza o novo tempo restante */
}

int MAXHEAPtop(MaxHeap H)
{
    if (H->size == 0) return -1;
    return H->heap[1];
}

void MAXHEAPremove(MaxHeap H, Process procs[], double ct, int id)
{
    procs[H->heap[H->msk[id]]].pt = -CORNER;
    MAXHEAPupdate(H->heap, procs, H->msk, ct, H->msk[id]);
    MAXHEAPpop(H, procs, ct);
}
