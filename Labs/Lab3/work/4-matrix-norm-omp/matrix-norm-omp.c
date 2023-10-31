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
    int n_threads = 4;

    if (argc >= 2) {
        n_threads = atoi(argv[1]);
    }
    if (argc >= 3) {
        size = atoi(argv[2]);
    }
    printf("Usage: %s {n_threads} {n}\n", argv[0]);
    printf("\twhere n_threads is the number of threads to set, and n is the matrix size parameter.\n");
    printf("I will be using %u threads, and size = %u\n", n_threads, size);

    int n = size;
    int m = (int)(n * 1.1);
    int p = (int)(n * 0.9);

    mat_t *e = mat_new(m, n); 
    mat_fillPattern(e);
    mat_t *f = mat_new(n, p);
    mat_fillPattern(f);

    omp_set_num_threads(n_threads);

    double norm_ref;
    double norm;
    norm_ref = mat_norm_ij(e);

    unsigned long long t0 = micros();
    norm = mat_norm_omp(e);
    unsigned long long tf = micros();
    
    printf("I've computed the norm in %.6f seconds; the value is %scorrect\n\n", 
        1.0e-6 *(tf - t0), (fabs(norm-norm_ref) < 1.0e-9) ? "" : "in");

    if (n_threads == 1) {
        fprintf(stderr, "file; n_threads; time; error\n");
    }
    fprintf(stderr, "%s; %u; %.6f; %.6f\n", __FILE__, n_threads, 1.0e-6 * (tf - t0), norm-norm_ref);
    return 0;
}
