#include <stdio.h>
#include <stdlib.h> 

#include "utility.h"
#include "minheap.h"


static int minHeapCompare(int x, int y, Process *procs) 
{
    if (procs[x].rt > procs[y].rt) return 1;
    return 0;
}

MinHeap minHeapInit(int n) 
{
    int i;
    MinHeap H;

    H = malloc(sizeof *H);
    H->heap = malloc(2 * n * sizeof(int));
    H->msk = malloc((n + 2) * sizeof(int));
    for (i = 0; i <= n; i++) H->msk[i] = H->heap[i] = -1;
    H->size = 0;
    
    return H;
}

static void minHeapUpdate(int *heap, int *msk, Process procs[], int i) 
{
    int parent;

    if (i == 1) return;
    parent = i / 2;

    if (minHeapCompare(heap[parent], heap[i], procs)) {
        msk[heap[parent]] = i;
        msk[heap[i]] = parent;
        swap(&heap[parent], &heap[i]);
        minHeapUpdate(heap, msk, procs, parent);
    }
}

void minHeapInsert(MinHeap H, Process procs[], int id) 
{
    (H->size)++;
    H->heap[H->size] = id; 
    H->msk[id] = H->size;
    minHeapUpdate(H->heap, H->msk, procs, H->size);
}

static void minHeapSet(int heap[], int *msk, Process procs[], int i, int size)
{
    int child1, child2;

    if (i * 2 > size) return;

    child1 = 2 * i; 
    child2 = 2 * i + 1;

    if (child1 == size) child2 = -1;
    if (child2 != -1 && minHeapCompare(heap[child1], heap[child2], procs) 
        && minHeapCompare(heap[i], heap[child2], procs)) {
        msk[heap[i]] = child2;
        msk[heap[child2]] = i;
        swap(&heap[i], &heap[child2]);
        minHeapSet(heap, msk, procs, child2, size);
    }
    else if (minHeapCompare(heap[i], heap[child1], procs)) {
        msk[heap[i]] = child1;
        msk[heap[child1]] = i;
        swap(&heap[i], &heap[child1]);
        minHeapSet(heap, msk, procs, child1, size);
    }
}

void minHeapPop(MinHeap H, Process procs[])
{
    int pop;

    if (H->size == 0) return;
    
    pop = H->heap[1];
    H->heap[1] = H->heap[H->size];
    (H->size)--; 
    minHeapSet(H->heap, H->msk, procs, 1, H->size);
    H->msk[pop] = -1;
}

int minHeapTop(MinHeap H)
{
    if (H->size == 0) return -1;
    return H->heap[1];
}

void minHeapRemove(MinHeap H, Process procs[], int id)
{
    minHeapUpdate(H->heap, H->msk, procs, H->msk[id]);
    minHeapPop(H, procs);
}