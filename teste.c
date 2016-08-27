#include <stdio.h>
#include <stdlib.h>
#include "heap_min.h"
#include "heap_max.h"
#include "utility.h"

int n;
double ct;
HeapMin H;

void show() {
    int a;
    for (a = 1; a <= H->sz; a++)
        printf("%s-%d ", procs[H->heap[a]].name, H->msk[H->heap[a]]);
    printf("\n");
}

int main() {
    int i;
    readFile("in.txt", &n, procs);
    printf("N = %d\n", n);
    H = HEAPMNinit(n);
    ct = 0.0;
    for (i = 0; i < n; i++) {
        HEAPMNinsert(H, procs, i);
    }
    show();
    i = HEAPMNpop(H, procs);
    while (i != -1) {
        show();
        i = HEAPMNpop(H, procs);
    }
    return 0;
}