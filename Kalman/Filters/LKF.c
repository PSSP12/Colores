#include <stdio.h>
#include <stdlib.h>
#include "../utils/matrix.c"  // Suponemos que matrix.h contiene las declaraciones de las funciones en matrix.c
#include <sys/sysinfo.h>

// Prediction
double *temp, *F_transpose, *FP, *FPF_transpose, *FP, *FPF_transpose, *new_P;

// Correction
double *Hx, *H_transpose, *HP, *HPH_transpose, *S, *S_inv, *PH_transpose, *K, *Ky, *KH, *I_KH, *minus_KH;

// Stability
bool started = false;

typedef struct {
    double *x;  // State estimate vector
    double *P;  // Estimate covariance matrix
    double *F;  // State transition matrix
    double *Q;  // Process noise covariance matrix
    double *H;  // Observation matrix
    double *R;  // Observation noise covariance matrix
} KalmanFilter;

void initialize(int obs_size, int state_size) {
    if (!started) {
        // Stability
        started = true;

        // Prediction
        temp = (double *)calloc(state_size, sizeof(double));  // Vector de estados temporal
        F_transpose = (double *)calloc(state_size*state_size, sizeof(double));
        FP = (double *)calloc(state_size*state_size, sizeof(double));
        FPF_transpose = (double *)calloc(state_size*state_size, sizeof(double));
        new_P = (double *)calloc(state_size*state_size, sizeof(double));

        // Correction
        Hx = (double *)calloc(obs_size*state_size, sizeof(double));
        H_transpose = (double *)calloc(obs_size*state_size, sizeof(double));
        HP = (double *)calloc(obs_size*state_size, sizeof(double));
        HPH_transpose = (double *)calloc(obs_size*obs_size, sizeof(double));
        S = (double *)calloc(obs_size*obs_size, sizeof(double));
        S_inv = (double *)calloc(obs_size*obs_size, sizeof(double));
        PH_transpose = (double *)calloc(state_size*obs_size, sizeof(double));
        K = (double *)calloc(state_size*obs_size, sizeof(double));
        Ky = (double *)calloc(state_size*obs_size, sizeof(double));
        KH = (double *)calloc(state_size*obs_size, sizeof(double));
        I_KH = (double *)calloc(state_size*state_size, sizeof(double));
        minus_KH = (double *)calloc(state_size*obs_size, sizeof(double));
    }
}

void kalman_predict(KalmanFilter *kf, int size) {
    int state_size[2] = {size, size};
    int vector_size[2] = {size, 1};
    
    // x = F * x
    mat_product(state_size, vector_size, kf->F, kf->x, temp);
    kf->x = temp;
    
    // P = F * P * F^T + Q
    mat_transpose(state_size, kf->F, F_transpose);
    mat_product(state_size, state_size, kf->F, kf->P, FP);
    mat_product(state_size, state_size, FP, F_transpose, FPF_transpose);

    mat_sum(state_size, FPF_transpose, kf->Q, new_P);
    kf->P = new_P;
}

void kalman_update(KalmanFilter *kf, double *z, int size, int o_size) {
    int state_size[2] = {size, size};
    int vector_size[2] = {size, 1};
    int H_size[2] = {o_size, size};
    int H_size_t[2] = {size, o_size};
    int obs_size_2[2] = {o_size, o_size};
    
    // y = z - H * x
    mat_product(H_size, vector_size, kf->H, kf->x, Hx);
    double *y = (double *)malloc(o_size * sizeof(double));
    for (int i = 0; i < o_size; i++) {
        y[i] = z[i] - Hx[i];
    }
    
    // S = H * P * H^T + R
    mat_transpose(H_size, kf->H, H_transpose);
    mat_product(H_size, state_size, kf->H, kf->P, HP);
    mat_product(H_size, H_size_t, HP, H_transpose, HPH_transpose);
    
    mat_sum(obs_size_2, HPH_transpose, kf->R, S);
    
    // K = P * H^T * S^(-1)
    mat_inv(obs_size_2, S, S_inv);
    
    mat_transpose(H_size, kf->H, H_transpose);
    mat_product(state_size, H_size_t, kf->P, H_transpose, PH_transpose);
    
    mat_product(H_size_t, obs_size_2, PH_transpose, S_inv, K);
    
    // x = x + K * y
    mat_product(H_size_t, obs_size_2, K, y, Ky);
    for (int i = 0; i < size; i++) {
        kf->x[i] += Ky[i];
    }
    
    // P = (I - K * H) * P
    mat_product(H_size_t, H_size, K, kf->H, KH);
    double *I = (double *)malloc(size * size * sizeof(double));
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            I[i * size + j] = (i == j) ? 1.0 : 0.0;
        }
    }
    mat_num(-1.0, state_size, KH, minus_KH);
    mat_sum(state_size, I, minus_KH, I_KH);
    
    mat_product(state_size, state_size, I_KH, kf->P, new_P);
    kf->P = new_P;
}

void finalize() {
    if (started) {
        started = false;
        free(temp); free(F_transpose); free(FP); free(FPF_transpose); free(new_P); free(Hx); free(H_transpose); free(HP); free(HPH_transpose); free(S); free(S_inv); free(PH_transpose); free(K); free(Ky); free(KH); free(I_KH); free(minus_KH);
    }
}