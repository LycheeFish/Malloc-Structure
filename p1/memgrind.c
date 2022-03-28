#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int main(int argc, char**argv) {
    int *p = malloc(1024);

    //free(p);

    int *e = malloc(8);
    int *f = malloc(16);
    int *g = malloc(32);
    int *h = malloc(16);

    free(p);
    free(h);
    free(f+1);

    int a = 3;
    int *q = &a;
    free(q);

    return EXIT_SUCCESS;
}