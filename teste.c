#include <stdio.h>
#include <stdlib.h>
#include "heap_min.h"
#include "heap_max.h"
#include "utility.h"

int n;
double ct;
MinHeap H;

void show() {
    int a;
    for (a = 1; a <= H->size; a++)
        printf("%s-%d ", procs[H->heap[a]].name, H->msk[H->heap[a]]);
    printf("\n");
}

int main() {
    int i;
    readFile("in.txt", &n, procs);
    printf("N = %d\n", n);
    H = MINHEAPinit(n);
    ct = 0.0;
    for (i = 0; i < n; i++) {
        MINHEAPinsert(H, procs, i);
    }
    show();
    i = MINHEAPpop(H, procs);
    while (i != -1) {
        show();
        i = MINHEAPpop(H, procs);
    }
    return 0;
}