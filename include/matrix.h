/**
 * @file matrix.c
 * @author Pablo Sánchez de Pablos
 * @brief Matrix operations for Kalman filter and other numerical algorithms.
 * @version 0.1
 * @date 2025-05-27
 * 
 * @details
 * This module provides basic matrix manipulation functions designed for efficient
 * operation on embedded systems, such as the LEGO Mindstorms EV3. It supports:
 *  - Matrix inversion via Gauss-Jordan elimination
 *  - Determinant calculation (recursive)
 *  - Matrix transposition
 *  - Matrix multiplication
 *  - Matrix addition
 *  - Scalar-matrix multiplication
 *  - Matrix printing
 * 
 * The code operates on matrices stored as **contiguous 1D arrays in row-major order**.
 * The shape of each matrix is passed explicitly as a 2-element integer array:
 * 
 *     int size[2] = {rows, cols};
 * 
 * For example, a 3×3 matrix A:
 * 
 *     double A[9] = {
 *         1.0, 2.0, 3.0,
 *         4.0, 5.0, 6.0,
 *         7.0, 8.0, 9.0
 *     };
 *     int size[2] = {3, 3};
 * 
 * You access element (i, j) via: `A[i * size[1] + j]`
 * 
 * The module favors **explicit memory handling and minimal dependencies**, to ensure
 * portability and predictability on low-resource environments. Error codes are
 * returned as integers: `0` on success, non-zero (or `NULL`/`NAN`) on failure.
 * 
 * @note Some functions (like `mat_inv`) internally allocate memory; always check return codes.
 */
#ifndef MATRIX_H
#define MATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>  /* for size_t if you prefer */

/**
 * @brief Multiply A (A_rows×A_cols) by B (B_rows×B_cols) into C (A_rows×B_cols).
 * @param sizeA [in]  {rows, cols} of A
 * @param sizeB [in]  {rows, cols} of B
 * @param A     [in]  pointer to A (row-major)
 * @param B     [in]  pointer to B (row-major)
 * @param C     [out] pointer to result (preallocated)
 * @return 0 on success, non-zero on dimension mismatch
 */
int mat_product(const int sizeA[2],
                const int sizeB[2],
                const double *A,
                const double *B,
                double *C);

/**
 * @brief Transpose A (rows×cols) into C (cols×rows).
 * @param size [in]  {rows, cols} of A
 * @param A    [in]  pointer to source matrix
 * @param C    [out] pointer to dest matrix (preallocated)
 * @return 0 on success
 */
int mat_transpose(const int size[2],
                  const double *A,
                  double *C);

/**
 * @brief Sum A and B into C. All must be same dimensions.
 * @param size [in]  {rows, cols}
 * @param A    [in]
 * @param B    [in]
 * @param C    [out] (preallocated)
 * @return 0 on success, non-zero on error
 */
int mat_sum(const int size[2],
            const double *A,
            const double *B,
            double *C);

/**
 * @brief Scale A by scalar into C.
 * @param scalar [in] factor
 * @param size   [in] {rows, cols}
 * @param A      [in]
 * @param C      [out] (preallocated)
 * @return 0 on success, non-zero on error
 */
int mat_num(double scalar,
            const int size[2],
            const double *A,
            double *C);

/**
 * @brief Invert square matrix A into inv via Gauss–Jordan.
 * @param size [in]  {n, n}
 * @param A    [in]  source matrix
 * @param inv  [out] inverse (preallocated n×n)
 * @return 0 on success, 1 on singular or error
 */
int mat_inv(const int size[2],
            const double *A,
            double *inv);

/**
 * @brief Compute determinant of square A via LU decomposition.
 * @param size [in]  {n, n}
 * @param A    [in]
 * @param D    [out] determinant
 * @return 0 on success, -1 if non-square, -2 on alloc failure
 */
int mat_det(const int size[2],
            const double *A,
            double *D);

/**
 * @brief Print matrix A to stdout with given decimal precision.
 * @param size      [in] {rows, cols}
 * @param A         [in]
 * @param precision [in] number of decimal places
 * @return 0 on success
 */
int mat_print(const int size[2],
              const double *A,
              int precision);

#ifdef __cplusplus
}
#endif

#endif /* MATRIX_H */
