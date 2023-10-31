/**
 * @file main.c
 * @brief Test setup for matrix lab
 * @author Etienne Hamelin
 * @date 26/10/2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include "matrix.h"
#include "utils.h"

/**
 * @brief Main test procedure
 */
int main(int argc, char **argv) {
    /* default values */
    int size = 10000;

    if (argc >= 2) {
        size = atoi(argv[1]);
    }
    printf("Usage: %s {size}\n", argv[0]);
    printf("\twhere size is indicative of the size of the matrices.\n");
    printf("I will be using size = %u\n", size);

    int n = size;
    int m = (int)(n * 1.1);

    mat_t *e = mat_new(m, n); 
    mat_fillPattern(e);

    TIMEIT(printf("mat_norm_ij: %12.3f\n", mat_norm_ij(e)););
    TIMEIT(printf("mat_norm_ji: %12.3f\n", mat_norm_ji(e)););

    return 0;
}
