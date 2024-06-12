#include <stdio.h>
#include <stdlib.h>
#include "matrix.c"  // Suponemos que matrix.h contiene las declaraciones de las funciones en matrix.c

typedef struct {
    double *x;  // State estimate vector
    double *P;  // Estimate covariance matrix
    double *F;  // State transition matrix
    double *Q;  // Process noise covariance matrix
    double *H;  // Observation matrix
    double *R;  // Observation noise covariance matrix
} KalmanFilter;

void kalman_predict(KalmanFilter *kf, int *size) {
    int state_size[2] = {size[0], size[0]};
    
    // x = F * x
    double *temp = mat_product(state_size, size, kf->F, kf->x);
    free(kf->x);
    kf->x = temp;
    
    // P = F * P * F^T + Q
    double *F_transpose = mat_transpose(state_size, kf->F);
    double *FP = mat_product(state_size, size, kf->F, kf->P);
    double *FPF_transpose = mat_product(state_size, state_size, FP, F_transpose);
    free(FP);
    free(F_transpose);
    
    double *new_P = mat_sum(state_size, FPF_transpose, kf->Q);
    free(kf->P);
    kf->P = new_P;
    free(FPF_transpose);
}

void kalman_update(KalmanFilter *kf, double *z, int *size, int *obs_size) {
    int state_size[2] = {size[0], size[0]};
    int obs_size_t[2] = {obs_size[1], obs_size[0]};
    
    // y = z - H * x
    double *Hx = mat_product(obs_size, size, kf->H, kf->x);
    double *y = (double *)malloc(obs_size[0] * sizeof(double));
    for (int i = 0; i < obs_size[0]; i++) {
        y[i] = z[i] - Hx[i];
    }
    free(Hx);
    
    // S = H * P * H^T + R
    double *H_transpose = mat_transpose(obs_size, kf->H);
    double *HP = mat_product(obs_size, state_size, kf->H, kf->P);
    double *HPH_transpose = mat_product(obs_size, obs_size_t, HP, H_transpose);
    free(HP);
    
    double *S = mat_sum(obs_size, HPH_transpose, kf->R);
    free(HPH_transpose);
    free(H_transpose);
    
    // K = P * H^T * S^(-1)
    double *S_inv = mat_inv(obs_size, S);
    free(S);
    
    H_transpose = mat_transpose(obs_size, kf->H);
    double *PH_transpose = mat_product(state_size, obs_size_t, kf->P, H_transpose);
    free(H_transpose);
    
    double *K = mat_product(state_size, obs_size, PH_transpose, S_inv);
    free(S_inv);
    free(PH_transpose);
    
    // x = x + K * y
    double *Ky = mat_product(state_size, obs_size, K, y);
    for (int i = 0; i < size[0]; i++) {
        kf->x[i] += Ky[i];
    }
    free(Ky);
    free(y);
    
    // P = (I - K * H) * P
    double *KH = mat_product(state_size, obs_size, K, kf->H);
    double *I = (double *)malloc(size[0] * size[0] * sizeof(double));
    for (int i = 0; i < size[0]; i++) {
        for (int j = 0; j < size[0]; j++) {
            I[i * size[0] + j] = (i == j) ? 1.0 : 0.0;
        }
    }
    double *I_KH = mat_sum(state_size, I, mat_num(-1.0, KH, state_size));
    free(I);
    free(KH);
    
    double *new_P = mat_product(state_size, state_size, I_KH, kf->P);
    free(I_KH);
    free(kf->P);
    kf->P = new_P;
    free(K);
}
