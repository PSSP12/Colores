#ifndef LKF_H
#define LKF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/**
 * @brief Structure representing a Linear Kalman Filter (LKF).
 * 
 * This structure contains all the necessary variables and buffers for the Kalman filter operations.
 * It includes user-provided matrices for state estimation, process noise, observation, and observation noise,
 * as well as internal buffers for intermediate calculations during the prediction and update steps.
 */
typedef struct {
    // User-provided variables for the Kalman filter
    double  *x,  // State estimate vector
            *P,  // Estimate covariance matrix
            *F,  // State transition matrix
            *Q,  // Process noise covariance matrix
            *H,  // Observation matrix
            *R;  // Observation noise covariance matrix
    int obs_size,   // Size of the observation vector
        state_size; // Size of the state vector

    // Internal buffers for the Kalman filter calculations
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
            *y;             // Innovation vector (z - Hx)
    bool started;  // Flag to indicate if the filter has been initialized

    // Sizes
    int F_size[2],  H_size[2],  Q_size[2],  R_size[2],  P_size[2],  x_size[2],  y_size[2],
        K_size[2],  S_size[2],  F_transpose_size[2], FP_size[2], FPF_transpose_size[2],
        H_transpose_size[2], HP_size[2], HPH_transpose_size[2],
        PH_transpose_size[2], KH_size[2], I_KH_size[2], minus_KH_size[2];
} LKF;

// Public API
int  lkf_create(LKF *kf, double *x, double *P, double *F, double *Q, double *H, double *R,
                int obs_size, int state_size);
void lkf_predict(LKF *kf);
void lkf_update(LKF *kf, const double *z);
int  lkf_destroy(LKF *kf);

#ifdef __cplusplus
}
#endif

#endif /* LKF_H */


