#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define KILO 1000
#define MILLION KILO*KILO
#define STEPS   100*MILLION

#define LOW_BOUND -1000.0f
#define HIGH_BOUND 1000.0f

double pi = 3.141592653f;
double sigma = 12.0f;
double mu = 1.5f;

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
 * @brief The main function
 */
int main(int argc, char **argv) {

    double res = integrate(&f, LOW_BOUND, HIGH_BOUND, STEPS);
    double expected = sigma * sqrt(2 * pi);

    printf("Result = %3.10f\n", res);
    printf("Expected %3.10f\n", expected);
    printf("Error = %e\n", res - expected);

    return 0;
}




