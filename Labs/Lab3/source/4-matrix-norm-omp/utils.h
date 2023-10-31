/**
 * @file utils.h
 * @brief Utilities, e.g. measuring time (interface)
 * @author Etienne Hamelin
 * @date 26/10/2023
 */
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#define TIMEIT(block)                                                              \
    do                                                                             \
    {                                                                              \
        unsigned long long t = -micros();                                          \
        block;                                                                     \
        t += micros();                                                             \
        fprintf(stderr, "%s:%d; %12.6f\n", __FILE__, __LINE__, (double)t / 1.0e6); \
    } while (0);

unsigned long long micros();

#endif