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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**
 * @brief Swaps two rows in matrices A and I.
 * 
 * @param A Matrix A
 * @param I Identity matrix I
 * @param size Size of the matrices (rows, columns)
 * @param row1 Row index of the first row to swap
 * @param row2 Row index of the second row to swap
 * @return void
 */
void swap_rows(double *A, double *I, int *size, int row1, int row2) {
    int n = size[0];
    for (int i = 0; i < n; i++) {   // Loop through each column
        double temp = A[row1 * n + i];  // Store the value of row1
        A[row1 * n + i] = A[row2 * n + i];  // Swap row1 with row2
        A[row2 * n + i] = temp; // Assign the stored value to row2
        
        temp = I[row1 * n + i]; // Store the value of row1 in I
        I[row1 * n + i] = I[row2 * n + i];  // Swap row1 with row2 in I
        I[row2 * n + i] = temp; // Assign the stored value to row2 in I
    }
}

/**
 * @brief Divides a row in matrices A and I by a divisor.
 * 
 * @param A Matrix A
 * @param I Identity matrix I
 * @param size Size of the matrices (rows, columns)
 * @param row Row index to divide
 * @param divisor Value to divide the row by
 * @return void
 */
void divide_row(double *A, double *I, int *size, int row, double divisor) {
    int n = size[0];
    for (int i = 0; i < n; i++) {   // Loop through each column
        A[row * n + i] /= divisor;  // Divide the row in A by the divisor
        I[row * n + i] /= divisor;  // Divide the row in I by the divisor
    }
}

/**
 * @brief Subtracts a multiple of one row from another in matrices A and I.
 * 
 * @param A Matrix A
 * @param I Identity matrix I
 * @param size Size of the matrices (rows, columns)
 * @param row1 Row index to subtract from
 * @param row2 Row index to subtract
 * @param multiplier Value to multiply row2 by before subtracting
 * @return void
 */
void subtract_rows(double *A, double *I, int *size, int row1, int row2, double multiplier) {
    int n = size[0];
    for (int i = 0; i < n; i++) {   // Loop through each column
        A[row1 * n + i] -= multiplier * A[row2 * n + i];    // Subtract the row2 multiplied by the multiplier from row1 in A
        I[row1 * n + i] -= multiplier * I[row2 * n + i];    // Subtract the row2 multiplied by the multiplier from row1 in I
    }
}

/**
 * @brief Inverts a square matrix A using Gauss-Jordan elimination.
 * 
 * @param size Size of the matrix (rows, columns)
 * @param A Matrix A to be inverted
 * @param inv Resulting inverse matrix, which should be preallocated with the correct size
 * @return int Error code (0 for success, 1 for failure)
 */
int mat_inv(int *size, double *A, double* inv) {
    int n = size[0];
    double *I = (double *)malloc(n * n * sizeof(double));
    double *A_copy = (double *)malloc(n * n * sizeof(double));

    // Check for square matrix
    if (n != size[1]) {
        fprintf(stderr, "ERROR: matrix must be square (%dx%d)\n", n, size[1]);
        free(I);
        free(A_copy);
        return 1; // Non-square matrix error
    }

    // Initialize I as the identity matrix and A_copy as a copy of A
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            I[i * n + j] = (i == j) ? 1.0 : 0.0;
            A_copy[i * n + j] = A[i * n + j];
        }
    }

    // Apply Gauss-Jordan elimination
    for (int i = 0; i < n; i++) {
        // Search for maximum in this column
        double maxEl = fabs(A_copy[i * n + i]);
        int maxRow = i;
        for (int k = i + 1; k < n; k++) {
            if (fabs(A_copy[k * n + i]) > maxEl) {
                maxEl = fabs(A_copy[k * n + i]);
                maxRow = k;
            }
        }

        // Swap maximum row with current row
        if (maxRow != i) {
            swap_rows(A_copy, I, size, i, maxRow);
        }

        // Make the diagonal contain all 1's
        double divisor = A_copy[i * n + i];
        if (divisor == 0) {
            printf("Matrix is singular and cannot be inverted.\n");
            free(I);
            free(A_copy);
            return 1; // Matrix is singular and cannot be inverted
        }
        divide_row(A_copy, I, size, i, divisor);

        // Make all rows below and above this one 0 in the current column
        for (int k = 0; k < n; k++) {
            if (k != i) {
                double multiplier = A_copy[k * n + i];
                subtract_rows(A_copy, I, size, k, i, multiplier);
            }
        }
    }

    // Copy the inverse matrix to the output
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            inv[i * n + j] = I[i * n + j];
        }
    }

    free(I);
    free(A_copy);
    return 0; // Return success
}

/**
 * @brief Calculates the determinant of a square matrix A using LU decomposition.
 * 
 * @param size Size of the matrix (rows, columns)
 * @param A Matrix A for which the determinant is to be calculated
 * @param D Pointer to store the determinant value
 * @return int Error code (0 for success, -1 for non-square matrix, -2 for memory allocation failure)
 */
int mat_det(int *size, const double *A, double *D) {
    int n = size[0], m = size[1];
    if (n != m) {
        fprintf(stderr, "ERROR: matrix must be square (%dx%d)\n", n, m);
        return -1;
    }

    // 1) Copy into working buffer M
    double *M = calloc(n*n, sizeof *M);
    if (!M) {
        fprintf(stderr, "ERROR: out of memory\n");
        return -2;
    }
    for (int i = 0; i < n*n; ++i) M[i] = A[i];

    int sign = +1;

    // 2) LU elimination with partial pivoting
    for (int k = 0; k < n; ++k) {
        // a) pivot selection
        int pivot = k;
        double maxv = fabs(M[k*n + k]);
        for (int i = k+1; i < n; ++i) {
            double v = fabs(M[i*n + k]);
            if (v > maxv) {
                maxv = v;
                pivot = i;
            }
        }
        if (maxv == 0.0) {
            // singular → det = 0
            *D = 0.0;
            free(M);
            return 0;
        }

        // b) swap rows (in M) if needed
        if (pivot != k) {
            for (int j = 0; j < n; ++j) {
                double tmp = M[k*n + j];
                M[k*n + j] = M[pivot*n + j];
                M[pivot*n + j] = tmp;
            }
            sign = -sign;
        }

        // c) eliminate below
        for (int i = k+1; i < n; ++i) {
            double factor = M[i*n + k] / M[k*n + k];
            M[i*n + k] = 0.0;
            for (int j = k+1; j < n; ++j) {
                M[i*n + j] -= factor * M[k*n + j];
            }
        }
    }

    // 3) form determinant
    double det = (sign > 0 ? 1.0 : -1.0);
    for (int i = 0; i < n; ++i)
        det *= M[i*n + i];

    free(M);
    *D = det;
    return 0;
}

/**
 * @brief Transposes a matrix A and stores the result in C.
 * 
 * @param size Size of the matrix (rows, columns)
 * @param A Matrix A to be transposed
 * @param C Result matrix C, which should be preallocated with the correct size
 * @return int Error code (0 for success, 1 for failure)
 */
int mat_transpose(int *size, double *A, double* C){
    // Transpose a matrix
    int i; int o;   // Loop indices

    for (i = 0; i < size[1]; ++i) { // Loop through columns of A
        for (o = 0; o < size[0]; ++o) { // Loop through rows of A
            C[i*size[0] + o] = A[i + o*size[1]];    // Assign transposed value to C
        }
    }
    return 0;   // Return success
}

/**
 * @brief Multiplies two matrices A and B, storing the result in C.
 * 
 * @param sizeA Size of matrix A (rows, columns)
 * @param sizeB Size of matrix B (rows, columns)
 * @param A Matrix A
 * @param B Matrix B
 * @param C Result matrix C, which should be preallocated with the correct size
 * @return int Error code (0 for success, 1 for failure)
 */
int mat_product(int *sizeA, int *sizeB, double* A, double* B, double* C) {

    // Product calculations
    for (int i = 0; i < sizeA[0]; ++i) {    // Loop through rows of A
        for (int p = 0; p < sizeB[1]; ++p) {    // Loop through columns of B
            double s = 0;   // Initialize sum for the current element
            for (int o = 0; o < sizeA[1]; ++o) {    // Loop through columns of A and rows of B
                s += A[i * sizeA[1] + o] * B[o * sizeB[1] + p];
            }
            C[i * sizeB[1] + p] = s;    // Store the result in C
        }
    }

    return 0;   // Return success
}

/**
 * @brief Sums two matrices A and B, storing the result in C.
 * 
 * @param size Size of the matrices (rows, columns)
 * @param A Matrix A
 * @param B Matrix B
 * @param C Result matrix C, which should be preallocated with the correct size
 * @return int Error code (0 for success, 1 for failure)
 */
int mat_sum(int *size, double *A, double *B, double *C) {
    // Basic argument validation
    if (size == NULL || A == NULL || B == NULL || C == NULL) {
        return 1;  // null‐pointer error
    }
    int rows = size[0];
    int cols = size[1];
    if (rows <= 0 || cols <= 0) {
        return 2;  // invalid dimensions
    }

    // Compute total number of elements
    size_t n = (size_t)rows * (size_t)cols;

    // Pointer‐stream implementation
    double *pa = A, *pb = B, *pc = C;
    double *end = A + n;  // one past the last element

    for (; pa < end; ++pa, ++pb, ++pc) {
        *pc = *pa + *pb;
    }

    return 0;  // success
}

/**
 * @brief Multiplies a matrix A by a scalar a, storing the result in C.
 * 
 * @param a Scalar value to multiply with
 * @param size Size of the matrix (rows, columns)
 * @param A Matrix A
 * @param C Result matrix C, which should be preallocated with the correct size
 * @return int  0 on success, non-zero error code on failure
 */
int mat_num(double a, int *size, double *A, double *C) {
    // 1) Null-pointer checks
    if (!size || !A || !C) {
        return 1;  // null-pointer error
    }

    // 2) Dimension checks
    int rows = size[0];
    int cols = size[1];
    if (rows <= 0 || cols <= 0) {
        return 2;  // invalid dimensions
    }

    // 3) Compute total element count
    size_t n = (size_t)rows * (size_t)cols;

    // 4) Pointer-stream multiply
    double *pA = A, *pC = C;
    double *end = A + n;           // one past last element of A
    for (; pA < end; ++pA, ++pC) {
        *pC = a * *pA;
    }

    return 0;  // success
}

/**
 * @brief Prints a matrix A of size defined by size to a given precision.
 * 
 * @param size Size of the matrix (rows, columns)
 * @param A Matrix A to be printed
 * @param precision Number of decimal places to print
 * @return int Error code (0 for success)
 */
int mat_print(int* size, double* A, int precision) {
    int i, o;
    for(i = 0; i < size[0]; ++i) { 
        for(o = 0; o < size[1]; ++o) {
            printf("%.*f\t", precision, A[i*size[1]+o]);
        }
        printf("\n");
    }
    return 0;
}
