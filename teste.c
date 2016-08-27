#include <stdio.h>
#include <stdlib.h>
#include "heap_min.h"
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
    H = HEAPinit(n);
    ct = 0.0;
    for (i = 0; i < n; i++) {
        //procs[i].pt = procs[i].at;
        //printf("Inserindo %f %s %f %f\n", procs[i].pt, procs[i].name, procs[i].dt, procs[i].dl);
        HEAPinsert(H, procs, i);
    }
    show();
    i = HEAPpop(H, procs);
    while (i != -1) {
        show();
        //printf("Retornou %d\n", i);
        //printf("Retirando %s %f\n", procs[i].name, procs[i].rt);
        i = HEAPpop(H, procs);
    }
}