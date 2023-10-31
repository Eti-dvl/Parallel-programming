/**
 * @file utils.c
 * @brief Utilities, e.g. measuring time (implementation)
 * @author Etienne Hamelin
 * @date 26/10/2023
 */
#include <time.h>
#include "utils.h"

/**
 * @brief Get a timestamp with microseconds granularity
 * @return the timestamp
 */
unsigned long long micros() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    /* Convert the seconds/nanoseconds fields into a single microseconds value */
    return ts.tv_sec * 1e6 + ts.tv_nsec / 1e3;
}