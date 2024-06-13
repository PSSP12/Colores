#include "utils/matrix.c"
// Tests for the matrix module

void printer(int* size, double* A) {
    int i, o;
    for(i = 0; i < size[0]; ++i) { 
        for(o = 0; o < size[1]; ++o) {
            printf("%f\t", A[i*size[0]+o]);
        }
        printf("\n");
    }
}

int main(void) {
    double A [3][3] = {
        {1, 3, 2},
        {3, 2, 4},
        {5, 2, 5}
    };
    int a_size [2] = {3, 3};
    printer((int*)&a_size, (double*)&A); 
}