/**
 * @file matrix.h
 * @brief Header for matrix utilities
 * @author Etienne Hamelin
 * @date 26/10/2023
 */
#ifndef MATRIX_H
#define MATRIX_H

/**
 * @brief a macro to help access elements of a matrix (in row-major order)
 */
#define MAT(m, i, j)    ((m)->data[(i) * (m)->cols + (j)])


/**
 * @brief a matrix object
 */
typedef struct {
    int rows;
    int cols;
    double *data;
} mat_t;


mat_t *mat_new_data(int rows, int cols, double *data);
mat_t *mat_new(int rows, int cols);
mat_t *mat_fillRand(mat_t *self);
mat_t *mat_fillPattern(mat_t *self);
void mat_destroy(mat_t *self);
void mat_print(const mat_t *self);
double mat_compare(const mat_t *self, mat_t *other);
mat_t *mat_transpose(mat_t *self, const mat_t *other);
mat_t *mat_mult(mat_t *res, const mat_t *a, const mat_t *b);
mat_t *mat_mult_omp(mat_t *res, const mat_t *a, const mat_t *b);




#endif // MATRIX_H