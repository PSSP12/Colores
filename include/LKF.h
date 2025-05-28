#ifndef LKF_H
#define LKF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "matrix.h"

/**
 * @brief Opaque handle to a Linear Kalman Filter.
 */
typedef struct KalmanFilter KalmanFilter;

/**
 * @brief Allocate and initialize the Kalman filter.
 * @return 0 on success, non-zero on error
 */
int lkf_create(KalmanFilter *kf,
               double *x,
               double *P,
               double *F,
               double *Q,
               double *H,
               double *R,
               int obs_size,
               int state_size);

/**
 * @brief Perform the prediction step: x ← F·x,  P ← F·P·Fᵀ + Q
 */
void lkf_predict(KalmanFilter *kf);

/**
 * @brief Perform the update step with a new observation z.
 */
void lkf_update(KalmanFilter *kf, const double *z);

/**
 * @brief Free all internal buffers.  After this, the KalmanFilter struct can be
 *        freed by the caller if it was dynamically allocated.
 */
int lkf_destroy(KalmanFilter *kf);

#ifdef __cplusplus
}
#endif

#endif /* LKF_H */

