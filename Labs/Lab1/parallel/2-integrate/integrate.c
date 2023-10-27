#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define KILO 1000
#define MILLION KILO*KILO
#define STEPS   100*MILLION

#define LOW_BOUND -1000.0f
#define HIGH_BOUND 1000.0f
#define NUM_THREADS 20

double pi = 3.141592653f;
double sigma = 12.0f;
double mu = 1.5f;

/**
 * Thread structure parameters
 */
typedef struct ThreadParams {
    double low;
    double high;
    double (*function)(double x);
    unsigned long int steps;
    double result;
}ThreadParams_t;

/**
 * @brief a Gaussian function
 */
double f(double x) {
    return exp(-(x - mu) * (x - mu) / (2 * sigma * sigma));
}

/**
 * @brief Compute the integral of a function between two bounds
 */
double integrate(double (*function)(double x), double low, double high, unsigned long int steps) {
    double res = 0.0f;
    for (int n = 0; n < steps; n++) {
        double x0 = low + n * (high - low) / steps;
        double x1 = low + (n + 1) * (high - low) / steps;
        double y0 = function(x0);
        double y1 = function(x1);
        res += (x1 - x0) * (y0 + y1) / 2.0f;
    }
    return res;
}

/**
 * Function called by each thread
 */
void *threadFunction(void *arg) {
    struct ThreadParams *params = (struct ThreadParams *)arg;
    params->result = integrate(params->function, params->low, params->high, params->steps);
    pthread_exit(NULL);
}

/**
 * @brief The main function
 */
int main(int argc, char **argv) {

    int numThreads = NUM_THREADS;  // Dedault
    
    if (argc > 1) {
        numThreads = atoi(argv[1]);
        printf("Number of threads : %d\n", numThreads);
    }

    pthread_t threads[numThreads];
    ThreadParams_t threadParams[numThreads];

    for (int i = 0; i < numThreads; i++) {
        // Répartir l'intervalle entre les threads
        threadParams[i].low = LOW_BOUND + (i * (HIGH_BOUND - LOW_BOUND) / numThreads);
        threadParams[i].high = LOW_BOUND + ((i + 1) * (HIGH_BOUND - LOW_BOUND) / numThreads);
        threadParams[i].function = &f;
        threadParams[i].steps = STEPS / numThreads;

        pthread_create(&threads[i], NULL, threadFunction, (void *)&threadParams[i]);
    }

    double totalResult = 0.0;

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        totalResult += threadParams[i].result;
    }
    double expected = sigma * sqrt(2 * pi);

    printf("Result = %3.10f\n", totalResult);
    printf("Expected %3.10f\n", expected);
    printf("Error = %e\n", totalResult - expected);

    return 0;
}




