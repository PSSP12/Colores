#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// Basic matrix operations

void swap_rows(double *A, double *I, int *size, int row1, int row2) {
    int n = size[0];
    for (int i = 0; i < n; i++) {
        double temp = A[row1 * n + i];
        A[row1 * n + i] = A[row2 * n + i];
        A[row2 * n + i] = temp;
        
        temp = I[row1 * n + i];
        I[row1 * n + i] = I[row2 * n + i];
        I[row2 * n + i] = temp;
    }
}

void divide_row(double *A, double *I, int *size, int row, double divisor) {
    int n = size[0];
    for (int i = 0; i < n; i++) {
        A[row * n + i] /= divisor;
        I[row * n + i] /= divisor;
    }
}

void subtract_rows(double *A, double *I, int *size, int row1, int row2, double multiplier) {
    int n = size[0];
    for (int i = 0; i < n; i++) {
        A[row1 * n + i] -= multiplier * A[row2 * n + i];
        I[row1 * n + i] -= multiplier * I[row2 * n + i];
    }
}

double* mat_inv(int *size, double *A) {
    int n = size[0];
    double *I = (double *)malloc(n * n * sizeof(double));
    double *A_copy = (double *)malloc(n * n * sizeof(double));
    double *inv = (double *)malloc(n * n * sizeof(double));

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
            printf("La matriz no es invertible.\n");
            free(I);
            free(A_copy);
            return NULL; // Matrix is singular and cannot be inverted
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
    return inv;
}

double mat_det(int *size, double *A) {
    
    double D = 0;
    
    int i, j, o, p;
    double* C = (double*)calloc((size[0]-1) * (size[1]-1), sizeof(double));

    if (size[0] == 1) {
        return *A;
    } else {
        for (i = 0; i < size[1]; ++i) {
            for (o = 1; o < size[0]; ++o) {
                for (p = 0, j = 0; p < size[1]; ++p) {
                    if (p != i) {
                        C[(o-1)*(size[1]-1)+j] = A[o*size[1]+p];
                        j++;
                    }
                }
            }
            int size2[] = {size[0]-1, size[1]-1};
            fflush(stdout);
            double v = mat_det(size2, C);
            D = D + pow(-1, i) * A[i] * v;
        }
        free(C);
        return D;
    }
}

double* mat_transpose(int *size, double *A){
    // Transpose a matrix

    int i; int o;
    double* C = (double*)calloc(1, size[0]*size[1]);

    for (i = 0; i < size[1]; ++i) {
        for (o = 0; o < size[0]; ++o) {
            C[i*size[0]+o] = A[i+o*size[1]];
        }
    }
    return C;
}

double* mat_product(int *sizeA, int *sizeB, double* A, double*B){
    // Matrix product

    int i; int o; int p;
    double* C = (double*)calloc(1, sizeA[0]*sizeB[1]);

    if (C == NULL){ // Check for allocation problems
        printf("ERROR: Couldn't allocate memory for some obscure reason... Be more careful!");
        return NULL;
    }

    // Product calculations
    double s;
    for (i = 0; i < sizeA[0]; ++i) {
        for (p = 0; p < sizeB[1]; ++p) {
            s = 0;
            for (o = 0; o <sizeA[1]; ++o) {
                s = s+A[i*sizeA[1]+o]*B[p+o*sizeB[1]];
            }
            C[i*sizeA[1]+p] = s;
        }
    }

    return C;
}

double* mat_sum(int *size, double *A, double *B){
    // Matrix sum

    int i;
    double* C = (double*)calloc(1, size[0]*size[1]);

    if (C == NULL){ // Check for memory allocations issues
        printf("ERROR: Couldn't allocate memory for some obscure reason... Be more careful!");
        return NULL;
    }
    // Sum
    for (i = 0; i < sizeof(A); ++i) {
        C[i] = A[i]+B[i];
    }
    
    return C;
}

double* mat_num(double a, double *A, int *size) {
    int i;
    double* C = (double*)calloc(1, size[0]*size[1]);
    for(i = 0; i < size[0]*size[1]; ++i) {
        C[i] = a*A[i];
    }
    return C;
}
