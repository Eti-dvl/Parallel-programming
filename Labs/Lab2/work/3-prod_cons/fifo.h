/**
 * \file fifo.c
 * \brief interface for FIFO queue module
 * \author Etienne Hamelin
 * \version 0.1
 * \date october 2023
 */

#ifndef FIFO_H
#define FIFO_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

enum {
    FIFO_OK = 0,
    FIFO_ERR_FULL = -1, 
    FIFO_ERR_EMPTY = -2
};

typedef struct {
    int* buffer;
    int size;
    int read_idx ;
    int write_idx;
} fifo_t;

fifo_t *fifo_new(int size);
int fifo_destroy(fifo_t *self);
int fifo_push(fifo_t *self, int item);
int fifo_pop(fifo_t *self, int *item);
int fifo_get(fifo_t *self, int n, int *item);
int fifo_full(fifo_t *self);
int fifo_empty(fifo_t *self);
int fifo_length(fifo_t *self);
void fifo_print(fifo_t *self);
int fifo_test();

#endif
