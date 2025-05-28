/**
 * @file LKF.c
 * @author Pablo Sánchez de Pablos
 * @brief Implementation of a Linear Kalman Filter (LKF) in C.
 * @version 0.1
 * @date 2025-05-28
 * 
 * 
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>   // for memcpy
#include <stdbool.h>  // for bool
#include <matrix.h>  // Include matrix operations header

/**
 * @brief Structure representing a Linear Kalman Filter (LKF).
 * 
 * This structure contains all the necessary variables and buffers for the Kalman filter operations.
 * It includes user-provided matrices for state estimation, process noise, observation, and observation noise,
 * as well as internal buffers for intermediate calculations during the prediction and update steps.
 */
typedef struct {
    /**
     * User-provided variables for the Kalman filter
     */
    double  *x,  // State estimate vector
            *P,  // Estimate covariance matrix
            *F,  // State transition matrix
            *Q,  // Process noise covariance matrix
            *H,  // Observation matrix
            *R;  // Observation noise covariance matrix
    int obs_size,   // Size of the observation vector
        state_size; // Size of the state vector

    /**
     * Internal buffers for the Kalman filter calculations
     * These buffers are used to store intermediate results during the prediction and update steps.
     */
    // Prediction
    double  *x_temp,        // Temporary state vector for prediction
            *F_transpose,   // Transpose of the state transition matrix
            *FP,            // Product of F and P
            *FPF_transpose, // Product of FP and F^T
            *new_P;         // New covariance matrix after prediction
    // Update
    double  *Hx,            // Predicted observation vector
            *H_transpose,   // Transpose of the observation matrix
            *HP,            // Product of H and P
            *HPH_transpose, // Product of HP and H^T
            *S,             // Innovation covariance matrix
            *S_inv,         // Inverse of the innovation covariance matrix
            *PH_transpose,  // Product of P and H^T
            *K,             // Kalman gain
            *Ky,            // Product of K and innovation vector
            *KH,            // Product of K and H
            *I_KH,          // Identity matrix minus KH
            *minus_KH,      // Negative product of K and H
            *I,             // Identity matrix
            *y;            // Innovation vector (z - Hx)
    bool started;  // Flag to indicate if the filter has been initialized
    // Sizes
    int F_size[2],  // Size of the state transition matrix (rows, columns)
        H_size[2],  // Size of the observation matrix (rows, columns)
        Q_size[2],  // Size of the process noise covariance matrix (rows, columns)
        R_size[2],  // Size of the observation noise covariance matrix (rows, columns)
        P_size[2],  // Size of the estimate covariance matrix (rows, columns)
        x_size[2],  // Size of the state estimate vector (rows, columns)
        y_size[2],  // Size of the innovation vector (rows, columns)
        K_size[2],  // Size of the Kalman gain matrix (rows, columns)
        S_size[2],  // Size of the innovation covariance matrix (rows, columns)
        F_transpose_size[2],   // Size of the transposed state transition matrix
        FP_size[2],            // Size of the product of F and P
        FPF_transpose_size[2], // Size of the product of FP and F^T
        H_transpose_size[2],   // Size of the transposed observation matrix
        HP_size[2],            // Size of the product of H and P
        HPH_transpose_size[2], // Size of the product of HP and H^T
        PH_transpose_size[2],  // Size of the product of P and H^T
        KH_size[2],            // Size of the product of K and H
        I_KH_size[2],          // Size of the identity matrix minus KH
        minus_KH_size[2];      // Size of the negative product of K and H
} KalmanFilter;

/**
 * @brief Initializes the Kalman filter with the given observation and state sizes.
 * 
 * @param kf Pointer to the KalmanFilter structure to initialize.
 * @param x Pointer to the state estimate vector.
 * @param P Pointer to the estimate covariance matrix.
 * @param F Pointer to the state transition matrix.
 * @param Q Pointer to the process noise covariance matrix.
 * @param H Pointer to the observation matrix.
 * @param R Pointer to the observation noise covariance matrix.
 * @param obs_size Size of the observation vector.
 * @param state_size Size of the state vector.
 * @return int Returns 0 on success, 1 if the filter has already been started.
 */
int lkf_create(KalmanFilter *kf, double *x, double *P, double *F, double *Q, double *H, double *R, int obs_size, int state_size) {
    // Initializes the Kalman filter with the provided matrices and sizes.
    if (kf == NULL || x == NULL || P == NULL || F == NULL || Q == NULL || H == NULL || R == NULL) {
        return 1;  // Null-pointer error
    }
    kf->x = x;  // State estimate vector
    kf->P = P;  // Estimate covariance matrix
    kf->F = F;  // State transition matrix
    kf->Q = Q;  // Process noise covariance matrix
    kf->H = H;  // Observation matrix
    kf->R = R;  // Observation noise covariance matrix
    kf->obs_size = obs_size;   // Size of the observation vector
    kf->state_size = state_size; // Size of the state vector

    // Allocates the neccessary memory for the used variables along the functions in this code
    if (kf->started) {
        // If the filter has already been started, we do not need to allocate memory again
        return 1;  // Already started error
    }

    // Prediction
    kf->x_temp = (double *)malloc(kf->state_size * sizeof(double));
    kf->F_transpose = (double *)malloc(kf->state_size * kf->state_size * sizeof(double));
    kf->FP = (double *)malloc(kf->state_size * kf->state_size * sizeof(double));
    kf->FPF_transpose = (double *)malloc(kf->state_size * kf->state_size * sizeof(double));
    kf->new_P = (double *)malloc(kf->state_size * kf->state_size * sizeof(double));

    // Update
    kf->Hx = (double *)malloc(kf->obs_size * sizeof(double));
    kf->H_transpose = (double *)malloc(kf->state_size * kf->obs_size * sizeof(double));
    kf->HP = (double *)malloc(kf->obs_size * kf->state_size * sizeof(double));
    kf->HPH_transpose = (double *)malloc(kf->obs_size * kf->obs_size * sizeof(double));
    kf->S = (double *)malloc(kf->obs_size * kf->obs_size * sizeof(double));
    kf->S_inv = (double *)malloc(kf->obs_size * kf->obs_size * sizeof(double));
    kf->PH_transpose = (double *)malloc(kf->state_size * kf->obs_size * sizeof(double));
    kf->K = (double *)malloc(kf->state_size * kf->obs_size * sizeof(double));
    kf->Ky = (double *)malloc(kf->state_size * sizeof(double));
    kf->KH = (double *)malloc(kf->state_size * kf->state_size * sizeof(double));
    kf->I_KH = (double *)malloc(kf->state_size * kf->state_size * sizeof(double));
    kf->minus_KH = (double *)malloc(kf->state_size * kf->state_size * sizeof(double));
    kf->I = (double *)malloc(kf->state_size * kf->state_size * sizeof(double));
    for (int i = 0; i < kf->state_size; i++) {
        for (int j = 0; j < kf->state_size; j++) {
            kf->I[i * kf->state_size + j] = (i == j) ? 1.0 : 0.0;
        }
    }
    kf->y = (double *)malloc(kf->obs_size * sizeof(double));

    // Set sizes for matrices
    kf->F_size[0] = kf->state_size;
    kf->F_size[1] = kf->state_size;
    kf->H_size[0] = kf->obs_size;
    kf->H_size[1] = kf->state_size;
    kf->Q_size[0] = kf->state_size;
    kf->Q_size[1] = kf->state_size;
    kf->R_size[0] = kf->obs_size;
    kf->R_size[1] = kf->obs_size;
    kf->P_size[0] = kf->state_size;
    kf->P_size[1] = kf->state_size;
    kf->x_size[0] = kf->state_size;
    kf->x_size[1] = 1;  // State vector is a column vector
    kf->y_size[0] = kf->obs_size;
    kf->y_size[1] = 1;  // Innovation vector is a column vector
    kf->K_size[0] = kf->state_size;
    kf->K_size[1] = kf->obs_size;  // Kalman gain is a matrix
    kf->S_size[0] = kf->obs_size;
    kf->S_size[1] = kf->obs_size;  // Innovation covariance matrix is square
    kf->F_transpose_size[0] = kf->state_size;
    kf->F_transpose_size[1] = kf->state_size;  // Transpose of F
    kf->FP_size[0] = kf->state_size;
    kf->FP_size[1] = kf->state_size;  // Product of F and
    kf->FPF_transpose_size[0] = kf->state_size;
    kf->FPF_transpose_size[1] = kf->state_size;  // Product of FP and F^T
    kf->H_transpose_size[0] = kf->state_size;
    kf->H_transpose_size[1] = kf->obs_size;  // Transpose of H
    kf->HP_size[0] = kf->obs_size;
    kf->HP_size[1] = kf->state_size;  // Product of H and
    kf->HPH_transpose_size[0] = kf->obs_size;
    kf->HPH_transpose_size[1] = kf->obs_size;  // Product of HP and H^T
    kf->PH_transpose_size[0] = kf->state_size;
    kf->PH_transpose_size[1] = kf->obs_size;  // Product of P and H^T
    kf->KH_size[0] = kf->state_size;
    kf->KH_size[1] = kf->obs_size;  // Product of K and H
    kf->I_KH_size[0] = kf->state_size;
    kf->I_KH_size[1] = kf->state_size;  // Identity matrix minus KH
    kf->minus_KH_size[0] = kf->state_size;
    kf->minus_KH_size[1] = kf->state_size;  // Negative product of K and H

    kf->started = true; // Mark the filter as started

    return 0;  // Return success
}

/**
 * @brief Predicts the next state of the Kalman filter.
 * 
 * @param kf Pointer to the KalmanFilter structure.
 * @param size Size of the state vector.
 */
void lkf_predict(KalmanFilter *kf) {
    // Prediction step of the filter
    
    // x = F * x
    mat_product(kf->F_size, kf->x_size, kf->F, kf->x, kf->x_temp);
    memcpy(kf->x, kf->x_temp, kf->state_size * sizeof(double));
    
    // P = F * P * F^T + Q
    mat_transpose(kf->F_size, kf->F, kf->F_transpose);
    mat_product(kf->F_size, kf->P_size, kf->F, kf->P, kf->FP);
    mat_product(kf->FP_size, kf->F_transpose_size, kf->FP, kf->F_transpose, kf->FPF_transpose);

    mat_sum(kf->Q_size, kf->FPF_transpose, kf->Q, kf->new_P);
    memcpy(kf->P, kf->new_P, kf->state_size * kf->state_size * sizeof(double));
}

/**
 * @brief Updates the Kalman filter with a new observation.
 * 
 * @param kf Pointer to the KalmanFilter structure.
 * @param z Pointer to the observation vector.
 * @param s_size Size of the state vector.
 * @param o_size Size of the observation vector.
 */
void lkf_update(KalmanFilter *kf, double *z) {
    // Update step of the filter
    
    // y = z - H * x
    mat_product(kf->H_size, kf->x_size, kf->H, kf->x, kf->Hx);
    for (int i = 0; i < kf->obs_size; i++) {
        kf->y[i] = z[i] - kf->Hx[i];
    }
    
    // S = H * P * H^T + R
    mat_transpose(kf->H_size, kf->H, kf->H_transpose);
    mat_product(kf->H_size, kf->P_size, kf->H, kf->P, kf->HP);
    mat_product(kf->HP_size, kf->H_transpose_size, kf->HP, kf->H_transpose, kf->HPH_transpose);
    
    mat_sum(kf->HPH_transpose_size, kf->HPH_transpose, kf->R, kf->S);
    
    // K = P * H^T * S^(-1)
    mat_inv(kf->S_size, kf->S, kf->S_inv);
    
    mat_product(kf->P_size, kf->H_transpose_size, kf->P, kf->H_transpose, kf->PH_transpose);
    
    mat_product(kf->PH_transpose_size, kf->S_size, kf->PH_transpose, kf->S_inv, kf->K);
    
    // x = x + K * y
    mat_product(kf->K_size, kf->y_size, kf->K, kf->y, kf->Ky);
    for (int i = 0; i < kf->state_size; i++) {
        kf->x[i] += kf->Ky[i];
    }
    
    // P = (I - K * H) * P
    mat_product(kf->K_size, kf->H_size, kf->K, kf->H, kf->KH);
    mat_num(-1.0, kf->KH_size, kf->KH, kf->minus_KH);
    mat_sum(kf->minus_KH_size, kf->I, kf->minus_KH, kf->I_KH);
    
    mat_product(kf->I_KH_size, kf->P_size, kf->I_KH, kf->P, kf->new_P);
    memcpy(kf->P, kf->new_P, kf->state_size * kf->state_size*sizeof(double));
}

/**
 * @brief Finalizes the Kalman filter by freeing allocated memory.
 * 
 * @param kf Pointer to the KalmanFilter structure to finalize.
 * @return int Returns 0 on success, 1 if the filter has not been started.
 */
int lkf_destroy(KalmanFilter *kf) {
    // Free up the previousy allocated memory
    if (!kf->started) {
        // If the filter has not been started, there is nothing to free
        return 1;  // Not started error
    }
    kf->started = false;
    free(kf->x_temp); 
    free(kf->F_transpose); 
    free(kf->FP); 
    free(kf->FPF_transpose); 
    free(kf->new_P); 
    free(kf->Hx); 
    free(kf->H_transpose); 
    free(kf->HP); 
    free(kf->HPH_transpose); 
    free(kf->S); 
    free(kf->S_inv); 
    free(kf->PH_transpose); 
    free(kf->K); 
    free(kf->Ky); 
    free(kf->KH); 
    free(kf->I_KH); 
    free(kf->minus_KH);
    free(kf->I);
    free(kf->y);
    return 0;
}