#include <stdio.h>
#include <stdlib.h> 

#include "utility.h"
#include "maxheap.h"


static int maxHeapCompare(int x, int y, Process *procs)
{
    if (procs[x].rt < procs[y].rt) return 1;
    return 0;
}

MaxHeap maxHeapInit(int n)
{
    int i;
    MaxHeap H;

    H = malloc(sizeof *H);
    H->heap = malloc(2 * n * sizeof(int));
    H->msk = malloc((n + 2) * sizeof(int));    
    for (i = 0; i <= n; i++) H->msk[i] = H->heap[i] = -1;
    H->size = 0;

    return H;
}

static void maxHeapUpdate(int *heap, int *msk, Process procs[], int i)
{
    int parent;

    if (i == 1) return;
    parent = i / 2;
    
    if (maxHeapCompare(heap[parent], heap[i], procs)) {
        msk[heap[parent]] = i;
        msk[heap[i]] = parent;
        swap(&heap[parent], &heap[i]);
        maxHeapUpdate(heap, msk, procs, parent);
    }
}

void maxHeapInsert(MaxHeap H, Process procs[], int id)
{
    (H->size)++;
    H->heap[H->size] = id; 
    H->msk[id] = H->size;
    maxHeapUpdate(H->heap, H->msk, procs, H->size);
}

static void maxHeapSet(int heap[], int *msk, Process procs[], int i, int size)
{
    int child1, child2;

    if (2 * i > size) return;

    child1 = 2 * i; 
    child2 = 2 * i + 1;
    
    if (child1 == size) child2 = -1;
    if (child2 != -1 && maxHeapCompare(heap[child1], heap[child2], procs) && maxHeapCompare(heap[i], heap[child2], procs)) {
        msk[heap[i]] = child2;
        msk[heap[child2]] = i;
        swap(&heap[i], &heap[child2]);
        maxHeapSet(heap, msk, procs, child2, size);
    }
    else if (maxHeapCompare(heap[i], heap[child1], procs)) {
        msk[heap[i]] = child1;
        msk[heap[child1]] = i;
        swap(&heap[i], &heap[child1]);
        maxHeapSet(heap, msk, procs, child1, size);
    }
}

void maxHeapPop(MaxHeap H, Process procs[])
{
    int pop;

    if (H->size == 0) return;
    
    pop = H->heap[1];
    H->heap[1] = H->heap[H->size];
    (H->size)--; 
    maxHeapSet(H->heap, H->msk, procs, 1, H->size);
    H->msk[pop] = -1;
}

int maxHeapTop(MaxHeap H)
{
    if (H->size == 0) return -1;
    return H->heap[1];
}

void maxHeapRemove(MaxHeap H, Process procs[], int id)
{
    maxHeapUpdate(H->heap, H->msk, procs, H->msk[id]);
    maxHeapPop(H, procs);
}
