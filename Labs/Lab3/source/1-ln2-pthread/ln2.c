#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define N_MAX 1000000000LL

#define THREAD_NUM 4
#define K_MAX 20

/* Global variables */
double sums[THREAD_NUM * K_MAX] __attribute__ ((aligned (64)));
int k;


/**
 * @brief Where most computation actually happens
 */
void *thread_worker(void *arg)
{
    int my_num = (intptr_t)arg; /* Interpret arg as an integer */
    long long n, n_min, n_max;

    n_min = my_num * N_MAX / THREAD_NUM;
    n_max = (my_num + 1) * N_MAX / THREAD_NUM;

    for (n = n_max; n > n_min; n--)
    {
        if (n % 2 == 0)
        {
            sums[my_num * k] -= 1.0 / (double)n;
        }
        else
        {
            sums[my_num * k] += 1.0 / (double)n;
        }
    }
    return NULL;
}

/**
 * @brief the Main entry point
 */
int main(int argc, char **argv)
{
    double sum;
    pthread_t tids[THREAD_NUM];

    k = 1;
    if (argc >= 2)
    {
        k = atoi(argv[1]);
    }
    k = (k < 1) ? 1 : (k > K_MAX) ? K_MAX : k;
    printf("Testing with k = %d\n", k);

    for (int i = 0; i < THREAD_NUM; i++)
    {
        sums[i * k] = 0.0;
        pthread_create(&tids[i], NULL, thread_worker, (void *)(intptr_t)i);
    }

    for (int i = THREAD_NUM - 1; i >= 0; i--)
    {
        pthread_join(tids[i], NULL);
        sum += sums[i * k];
    }
    printf("sum: %.12f\n", sum);
    return 0;
}
