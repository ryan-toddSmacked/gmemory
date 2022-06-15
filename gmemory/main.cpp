#include <iostream>

#include "src/gmemory.h"

#define ROWS (10)
#define COLS (0xFFFFF)


int main()
{


    setPrintOut(SILENCE_ACTIONS);

    double** mat = (double**)gmalloc(sizeof(double*) * ROWS);
    for (int i = 0; i < ROWS; i++)
        mat[i] = (double*)gmalloc(sizeof(double) * COLS);


    printHeap(stdout);

    for (int i = 0; i < ROWS; i++)
        gfree(mat[i]);

    gfree(mat);

    printHeap(stdout);

    G_ClearAll();


    return 0;
}

