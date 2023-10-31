/**
 * @file utils.h
 * @brief Utilities, e.g. measuring time (interface)
 * @author Etienne Hamelin
 * @date 26/10/2023
 */
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#define TIMEIT(block)                                                                      \
    do                                                                                     \
    {                                                                                      \
        unsigned long long t = -micros();                                                  \
        static unsigned long int count = 0;                                                \
        static long long sum = 0;                                                          \
        block;                                                                             \
        t += micros();                                                                     \
        ++count;                                                                           \
        sum += t;                                                                          \
        fprintf(stderr, "Block %s:%d run #%lu in %12.6fs (avg %12.6fs)\n",                 \
               __FILE__, __LINE__, count, (double)t / 1.0e6, (double)sum / count / 1.0e6); \
    } while (0);

unsigned long long micros();

#endif