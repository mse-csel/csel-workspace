#include <stdio.h>

#include "exercice/ex1-memory-oriented.c"
#define MEMORY_ORIENTED



int main() {
    int ret = 0;

#ifdef MEMORY_ORIENTED
    printf("--------------------------------------\n");
    printf("Exercice 1: Memory oriented exercice\n");
    ret = ex_memory_oriented();
#endif

    return ret;
}
