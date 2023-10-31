/**
 * @file matrix.c
 * @brief Matrix utilities
 * @author Etienne Hamelin
 * @date 26/10/2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <omp.h>
#include "matrix.h"

/**
 * @brief Generate a (pseudo-)random float using a uniform probability distribution
 * @param low lower bound
 * @param high higher bound
 * @return a random number x so that low <= x <= high
 */
double randUniform(double low, double high) {
    double s = (double)rand() / RAND_MAX;
    return s * low + (1.0 - s) * high;
}

/**
 * @brief Create a matrix from given data
 * @param rows number of rows
 * @param cols number of columns
 * @param data an array of rows*cols double
 * @return pointer to a matrix, or NULL (failed to allocate)
*/
mat_t *mat_new_data(int rows, int cols, double *data) {
    mat_t *self = calloc(1, sizeof(mat_t));
    if (!self) return NULL;
    self->rows = rows;
    self->cols = cols;
    self->data = data;
    return self;
}

/**
 * @brief Create matrix of zeros
 * @param rows number of rows
 * @param cols number of columns
 * @return pointer to a matrix, or NULL (failed to allocate)
 */
mat_t *mat_new(int rows, int cols) {
    double *data = calloc(rows * cols, sizeof(double)); 
    // note: calloc is similar to malloc, but additionally clears the data
    if (!data) {
        return NULL;
    }
    return mat_new_data(rows, cols, data);
}

/**
 * @brief Fill a matrix with random numbers from -1.0 to 1.0
 * @param self a matrix
 * @return self
 */
mat_t *mat_fillRand(mat_t *self) {
    assert(self);
    for (int i = 0; i < self->rows; i++) {
        for (int j = 0; j < self->cols; j++) {
            MAT(self, i, j) = randUniform(-1.0, 1.0);
        }
    }
    return self;
}

/**
 * @brief Fill a matrix with random numbers from -1.0 (upper/left corner) to 1.0 (down/right corner)
 * @param self a matrix
 * @return self
 * @note: M_ij = (i / (m - 1)) * (j / (n - 1))
 */
mat_t *mat_fillPattern(mat_t *self) {
    assert(self);
    for (int i = 0; i < self->rows; i++) {
        for (int j = 0; j < self->cols; j++) {
            MAT(self, i, j) = (double)i / (self->rows - 1) * (double)j / (self->cols - 1);
        }
    }
    return self;
}

/**
 * @brief Destructor
 * @param self a matrix
 */
void mat_destroy(mat_t *self) {
    if (self) {
        free(self->data);
        free(self);
    }
}

/**
 * @brief Print the content of a matrix to console
 * @param self a matrix
 */
void mat_print(const mat_t *self) {
    assert(self);
    printf("Matrix %p: \n", (void *)self);
    for (int i = 0; i < self->rows; i++) {
        printf(i ? "], \n [" : "[[");
        for (int j = 0; j < self->cols; j++) {
            printf("%9.3f, ", MAT(self, i, j));
        }
    }
    printf("]]\n");
}

/**
 * @brief Transpose a matrix
 * @param self the output matrix (will be overwritten)
 * @param other the input matrix to transpose
 * @return self
*/
mat_t *mat_transpose(mat_t *self, const mat_t *other) {
    assert(self);
    assert(other);
    assert(self->rows == other->cols);
    assert(self->cols == other->rows);
    for (int i = 0; i < self->rows; i++) {
        for (int j = 0; j < self->cols; j++) {
            MAT(self, i, j) = MAT(other, j, i);
        }
    }
    return self;
}

/**
 * @brief Compute the Frobenius norm of a matrix; in row-major order
 * @param self a matrix
 * @return the norm
 */
double mat_norm_ij(mat_t *self) {
    assert(self);
    double norm_sq = 0.0;
    for (int i = 0; i < self->rows; i++) {
        for (int j = 0; j < self->cols; j++) {
            norm_sq += MAT(self, i, j) * MAT(self, i, j);
        }
    }
    return sqrt(norm_sq);
}

/**
 * @brief Compute the Frobenius norm of a matrix; using OpenMP
 * @param self a matrix
 * @return the norm
 */
double mat_norm_omp(mat_t *self) {
    assert(self);

    /* Write your code here */

    return 0.0f;
}

/**
 * @brief Multiply two matrices (reference version)
 * @param res result matrix
 * @param a first multiplicand
 * @param b second multiplicand
 * @return res, or NULL if dimensions of a, b, res do not match
 * @note if a is a m×n matrix and b a n×p matrix, res should be m×p
 */
mat_t *mat_mult(mat_t *res, const mat_t *a, const mat_t *b) {
    /* Verify that we have valid pointers */
    assert(res);
    assert(a); 
    assert(b);
    /* Verify that matrix dimensions match */
    int m = a->rows;
    if (m != res->rows) return NULL;
    int n = a->cols;
    if (n != b->rows) return NULL;
    int p = b->cols;
    if (p != res->cols) return NULL;

    /* Actually compute the matrix multiplication */
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            MAT(res, i, j) = 0.0;
            for (int k = 0; k < n; k++) {
                MAT(res, i, j) += MAT(a, i, k) * MAT(b, k, j);
            }
        }
    }
    return res;
}

mat_t *mat_mult2(mat_t *res, const mat_t *a, const mat_t *b) {
    /* Verify that we have valid pointers */
    assert(res);
    assert(a); 
    assert(b);
    /* Verify that matrix dimensions match */
    int m = a->rows;
    if (m != res->rows) return NULL;
    int n = a->cols;
    if (n != b->rows) return NULL;
    int p = b->cols;
    if (p != res->cols) return NULL;

    /* Actually compute the matrix multiplication */
    #pragma omp parallel for
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            MAT(res, i, j) = 0.0;
            for (int k = 0; k < n; k++) {
                MAT(res, i, j) += MAT(a, i, k) * MAT(b, k, j);
            }
        }
    }
    return res;
}

mat_t *mat_mult3(mat_t *res, const mat_t *a, const mat_t *b) {
    /* Verify that we have valid pointers */
    assert(res);
    assert(a); 
    assert(b);
    /* Verify that matrix dimensions match */
    int m = a->rows;
    if (m != res->rows) return NULL;
    int n = a->cols;
    if (n != b->rows) return NULL;
    int p = b->cols;
    if (p != res->cols) return NULL;

    mat_t *bt = mat_new(b->cols, b->rows);
    assert(bt);
    mat_transpose(bt, b);

    /* Actually compute the matrix multiplication */
    #pragma omp parallel for
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            double tmp = 0.0;
            for (int k = 0; k < n; k++) {
                tmp += MAT(a, i, k) * MAT(bt, j, k);
            }
            MAT(res, i, j) = tmp;
        }
    }
    return res;
}

/**
 * @brief Check if two matrices are equal
 * @param self a matrix to compare
 * @param other second matrix to compare
 * @return 0 if they are equal (to 1.0e-9 tolerance), otherwise the signum of the first difference
 */
int mat_compare(const mat_t *self, mat_t *other) {
    assert(self->rows == other->rows);
    assert(self->cols == other->cols);
    for (int i = 0; i < self->rows; i++) {
        for (int j = 0; j < self->cols; j++) {
            double diff = MAT(self, i, j) - MAT(other, i, j);
            if (fabs(diff) > 1.0e-9) {
                return (diff > 0) ? 1 : -1;
            }
        }
    }
    return 0;
}
