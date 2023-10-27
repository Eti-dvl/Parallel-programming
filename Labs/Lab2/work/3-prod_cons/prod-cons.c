/**
 * \file prod-cons.c
 * \brief Lab2 exercice: multi-threaded producer/consumers
 * \author Etienne Hamelin
 * \version 0.1
 * \date 20 october 2023
 *
 */
#include <stdio.h>      // printf
#include <stdlib.h>     // malloc
#include <assert.h>     // assert
#include <pthread.h>    // pthread_create, pthread_join, ...
#include <stdbool.h>    // true/false
#include "fifo.h"       // fifo_new, fifo_push, fifo_pop, ...

#define N_ITEMS 1000000

/**
 * @brief Structure to pass arguments to/from main and worker threads
 */
typedef struct {
    pthread_t thread_id;
    unsigned int num;
    int count;
    char dummy[256];
} thread_arg_t;

/**
 * @brief fifo message queue (global variable)
 */
fifo_t *fifo;

/**
 * @brief stop flag (global variable)
 */
volatile bool stop;

/**
 * @brief Producer thread implementation
 * @param arg an opaque pointer to a thread_arg_t structure
 * @return NULL
 */
void *thread_prod(void *arg) {
    thread_arg_t *my_arg = (thread_arg_t *)arg;
    int rc;
    printf("Producer #%u starts\n", my_arg->num);
    // Produce my_arg->count items
    for (int i = 0; i < my_arg->count; i++) {
        do {
            rc = fifo_push(fifo, i);
            // push item onto fifo queue, and retry until success
        } while (rc != FIFO_OK);
    }
    printf("Producer #%u finishes, produced %u items\n", my_arg->num, my_arg->count);
    return NULL;
}

/**
 * @brief Consumer thread implementation
 * @param arg an opaque pointer to a thread_arg_t structure
 * @return NULL
 */
void *thread_cons(void *arg) {
    int rc;
    int item;
    thread_arg_t *my_arg = (thread_arg_t *)arg;
    printf("\t\tConsumer thread #%u\n", my_arg->num);

    my_arg->count = 0;
    while (true) {
        rc = fifo_pop(fifo, &item);
        if (rc == FIFO_OK) {
            // read from fifo is successful
            my_arg->count ++;
        } else if (stop) {
            // if fifo is empty AND stop flag is ascerted: terminate.
            break;
        }
    }
    printf("\t\tConsumer #%u: received %u items\n", my_arg->num, my_arg->count);
    return NULL;
}

/**
 * @brief Main procedure
 */
int main(int argc, char **argv) {
    int n_prod;
    int n_cons;
    stop = false;
    int total_produced = 0;
    int total_consumed = 0;

    if (argc < 3) {
        printf("Usage: %s n_prod n_cons, where n_prod is the number of producer threads, and n_cons the number of consumer threads. By default: 2 and 2.\n", argv[0]);
        n_prod = n_cons = 2;
    } else {
        n_prod = atoi(argv[1]);
        n_cons = atoi(argv[2]);
    }

    fifo = fifo_new(512);

    printf("Start with %u producers, and %u consumers\n", n_prod, n_cons);

    /* Allocate two argument arrays */
    thread_arg_t *prod_array = calloc(n_prod, sizeof(thread_arg_t));
    thread_arg_t *cons_array = calloc(n_cons, sizeof(thread_arg_t));

    /* Start producer threads */
    for (int i = 0; i < n_prod; i++) {
        prod_array[i].num = i;
        prod_array[i].count = (N_ITEMS * (i + 1) / n_prod) - (N_ITEMS * i / n_prod); /* this formula makes the sum of prod_array[i].count exactly equal N_ITEMS */
        pthread_create(&prod_array[i].thread_id, NULL, thread_prod, &prod_array[i]);
    }

    /* Start consumer threads */
    for (int i = 0; i < n_cons; i++) {
        cons_array[i].num = i;
        cons_array[i].count = 0;
        pthread_create(&cons_array[i].thread_id, NULL, thread_cons, &cons_array[i]);
    }

    /* Now wait for all producers to terminate */
    for (int i = 0; i < n_prod; i++) {
        pthread_join(prod_array[i].thread_id, NULL);
        total_produced += prod_array[i].count;
    }

    /* Signal consumers that it's over */
    stop = true;

    /* Now wait for all producers to terminate */
    for (int i = 0; i < n_cons; i++) {
        pthread_join(cons_array[i].thread_id, NULL);
        total_consumed += cons_array[i].count;
    }

    /* Display the statistics */
    printf("Total: %u produced, %u consumed. Lost/found: %d (%02d%%)\n", 
            total_produced, total_consumed, total_produced - total_consumed, 
            100 * (total_produced - total_consumed) / total_produced);

    return 0;
}
