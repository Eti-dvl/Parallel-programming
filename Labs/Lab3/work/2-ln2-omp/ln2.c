#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N_MAX 1000000000LL

int main (int argc, char **argv)
{
    double sum = 0.0;
    long long n;

    int n_threads = 1; /* default value */
    if (argc >= 2)
    {
        n_threads = atoi(argv[1]);
    }
    printf("Start with %d threads\n", n_threads);

#pragma omp parallel for num_threads(n_threads) reduction(+:sum)
    for (n = N_MAX; n > 0; n--)
    {
        if (n%2 == 0)
        {
            sum -= 1.0 / (double )n;
        }
        else
        {
            sum += 1.0 / (double ) n ;
        }
    }
    printf ("sum: %.12f\n" , sum) ;
    return 0;
}