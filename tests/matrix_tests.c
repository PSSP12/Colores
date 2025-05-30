#include <stdio.h>  // Include standard I/O for printing
#include <matrix.h>  // Include matrix operations header
// Tests for the matrix module

int main(void) {
    double A [3][3] = {
        {1, 3, 2},
        {3, 2, 4},
        {5, 2, 5}
    };
    int a_size [2] = {3, 3};
    double B [3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    int b_size [2] = {3, 3};
    double C [3][3] = {0}; // Result matrix for multiplication
    mat_print((int*)&a_size, (double*)&A, 2);
    double D;
    mat_det((int*)&a_size, (double*)&A, &D); 
    printf("Determinant: %f\n", D);
    mat_product((int*)&a_size, (int*)&b_size, (double*)&A, (double*)&B, (double*)&C);
    mat_print((int*)&a_size, (double*)&C, 2);
}