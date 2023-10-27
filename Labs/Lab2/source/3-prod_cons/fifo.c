/**
 * \file fifo.c
 * \brief implementation of FIFO queue module
 * \author Etienne Hamelin
 * \version 0.1
 * \date october 2023
 *
 */


#include "fifo.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/**
 * @brief Constructor
 * @param size the capacity of the buffer (note: max buffer length will be size-1)
 * @return pointer to fifo, or NULL
 */
fifo_t *fifo_new(int size) {
    fifo_t *self = malloc(sizeof(fifo_t));
    if (!self) {
        return NULL;
    }
    self->buffer = malloc(sizeof(int) * size);
    if (!self->buffer) {
        free(self);
        return NULL;
    }
    self->size = size;
    self->read_idx = 0;
    self->write_idx = 0;
    return self;
}

/**
 * @brief Destructor
 * @param self fifo to deallocate
 */
int fifo_destroy(fifo_t *self) {
    free(self->buffer);
    free(self);
    return FIFO_OK;
}

/**
 * @brief Push an item onto FIFO
 * @param selft a fifo
 * @param item an integer to store on fifo
 * @return FIFO_OK or FIFO_ERR_FULL
 */
int fifo_push(fifo_t *self, int item) {
    assert(self);
    if (fifo_full(self)) {
        return FIFO_ERR_FULL;
    }
    self->buffer[self->write_idx] = item;
    self->write_idx = (self->write_idx + 1) % self->size;
    return FIFO_OK;
}

/**
 * @brief Pop an item from FIFO
 * @param self a fifo
 * @param item pointer to an integer that will store the result
 * @return FIFO_OK or FIFO_ERR_EMPTY
 */
int fifo_pop(fifo_t *self, int *item) {
    assert(self); 
    assert(item);
    if (fifo_empty(self)) {
        return FIFO_ERR_EMPTY;
    }
    *item = self->buffer[self->read_idx];
    self->read_idx = (self->read_idx + 1) % self->size;
    return FIFO_OK;
}

/**
 * @brief Read (without popping!) the n-th item in fifo; where 0-th is the next item to pop
 * @param self a fifo
 * @param n index of item to read 
 * @param item pointer to an integer that will store the item read 
 * @return FIFO_OK, or FIFO_ERR_EMPTY if fifo doesn't have n items
 */
int fifo_peek(fifo_t *self, int n, int *item) {
    assert(self); 
    assert(item);
    if (fifo_length(self) <= n) {
        return FIFO_ERR_EMPTY;
    } 
    *item = self->buffer[(self->read_idx + n) % self->size];
    return FIFO_OK;
}

/**
 * @brief Is fifo full?
 * @param self a fifo
 * @return true (fifo is full) or false (there is at least 1 free cell)
 */
int fifo_full(fifo_t *self) {
    return fifo_length(self) >= (self->size - 1);
}

/**
 * @brief Is fifo empty?
 * @param self a fifo
 * @return true (fifo is empty) or false (there is at least 1 occupied cell)
 */
int fifo_empty(fifo_t *self) {
    return fifo_length(self) <= 0;
}

/**
 * @brief How many items are currently stored in fifo?
 * @param self a fifo
 * @return number of items stored (0 ... fifo->size-1)
 */
int fifo_length(fifo_t *self) {
    assert(self); 
    return (self->write_idx - self->read_idx + self->size) % self->size;
}

/**
 * @brief Print out the fifo content to stdout
 * @param self a fifo
 */
void fifo_print(fifo_t *self) {
    assert(self);
    printf("Fifo @%p (size %u, current length %u, read index %u, write index %u%s): [", 
            self, self->size, fifo_length(self), self->read_idx, self->write_idx,
            fifo_empty(self) ? ", empty" : fifo_full(self) ? ", full" : "");

    for (int i = 0; i < fifo_length(self); i++) {
        int item;
        int rc = fifo_peek(self, i, &item);
        assert(rc == FIFO_OK);
        printf("%s%d", i ? ", ": "", item);
    }
    printf("]\n");
}

/**
 * @brief Module test procedure
 * @return 0 if all tests pass
 */
int fifo_test() {
    int item;
    int rc;
    int capacity = 10;
    fifo_t * fifo = fifo_new(capacity);
    assert(fifo);

    /** 
     * Test an empty fifo: 
     *  fifo_empty() should be true, fifo_length() should be zero, fifo_peek() and fifo_pop() should return an error
     */
    assert(fifo_empty(fifo));
    assert(!fifo_full(fifo));
    assert(0 == fifo_length(fifo));
    assert(FIFO_ERR_EMPTY == fifo_peek(fifo, 0, &item));
    assert(FIFO_ERR_EMPTY == fifo_pop(fifo, &item));

    /** 
     * Fill fifo with capacity-1 items: fifo_push() should return OK, fifo_length() should return the number of items pushed
     */
    for (int i = 0; i < capacity-1; i++) {
        assert(!fifo_full(fifo));
        printf("\nPush %u\n", i);
        rc = fifo_push(fifo, i);
        assert(FIFO_OK == rc);
        assert(i+1 == fifo_length(fifo));
        assert(!fifo_empty(fifo));
    }

    /**
     * Test a full fifo:
     * fifo_full() should return true, fifo_empty() should be false, fifo_push() should fail with ERR_FULL
     */
    assert(fifo_full(fifo));
    assert(!fifo_empty(fifo));
    rc = fifo_push(fifo, 999);
    assert(FIFO_ERR_FULL == rc);
    assert(capacity - 1 == fifo_length(fifo));

    /**
     * Test fifo_peek()
     */
    for (int i = 0; i < capacity - 1; i++) {
        rc = fifo_peek(fifo, i, &item);
        assert(FIFO_OK == rc);
        assert(item == i);
    }

    /**
     * Test a full fifo:
     * fifo_full() should return true, fifo_empty() should be false, fifo_push() should fail with ERR_FULL
     */
    for (int i = 0; i < capacity-1; i++) {
        assert(!fifo_empty(fifo));
        rc = fifo_pop(fifo, &item);
        assert(FIFO_OK == rc);
        assert(item == i);
        rc = fifo_push(fifo, i + 100);
        assert(FIFO_OK == rc);
        fifo_print(fifo);
    }

    assert(capacity - 1 == fifo_length(fifo));

    /**
     * Test emptying the FIFO
     */
    for (int i = 0; i < capacity - 1; i++) {
        assert(!fifo_empty(fifo));
        rc = fifo_pop(fifo, &item);
        assert(FIFO_OK == rc);
        assert(item == 100 + i);
        fifo_print(fifo);
    }
    /**
     * Fifo should be empty again
     */
    assert(0 == fifo_length(fifo));
    assert(fifo_empty(fifo));

    fifo_destroy(fifo);

    printf("Test successful.\n");
    return 0;
}

#ifdef FIFO_TEST
int main(int argc, char **argv) {
    fifo_test();
    return 0;
}
#endif
