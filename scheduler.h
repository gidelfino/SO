#ifndef scheduler_H_
#define scheduler_H_

#include "utility.h"


/* Funcao que realiza uma operacao que consome tempo 
real da CPU para o processo de identificador tid 
(Ou seja, que a simula execucao de uma thread) */
void *timeOperation(void *tid);

/* First-Come First-Served: Funcao que escalona 
os n processos por ordem de chegada */
void firstCome(int n);


#endif