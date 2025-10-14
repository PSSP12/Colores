#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <LKF.h>

int main(void) {
	srand(time(NULL)); // seed the random number generator
	int i = 0;
	int i_max = 100;
	double x_max = 6.2;
	double dt = x_max / i_max; // time step
	double sigma = 0.9;
	FILE *file = fopen("./tests/TestingData/kalman_output.txt", "w");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file for writing\n");
		return EXIT_FAILURE;
	}
	fprintf(file, "Iteration\tReal X\tNoisy X\tEstimated X\tReal V\tEstimated V\n");

	// Example usage of the Kalman filter
	double x[2] =  {0.0,
			 		1.0};          // Initial state
	double P[4] =  {1e-6, 0,
					0, 1e-6};        // Initial covariance
	double F[4] =  {cos(dt), sin(dt), 
					-sin(dt), cos(dt)};        // State transition matrix
	double Q[4] =  {1e-6, 0,
				 	0, 1e-6};  // Process noise covariance
	double H[2] =  {1, 
					0};              // Observation matrix
	double R[1] = {4*sigma};               // Measurement noise covariance
	LKF kf;

	if (lkf_create(&kf, x, P, F, Q, H, R, 1, 2) != 0) {
		fprintf(stderr, "Failed to initialize Kalman filter\n");
		return EXIT_FAILURE;
	}

	for (i = 0; i < i_max; i++) {
		// Simulate a measurement
		double z = sin(dt*i) + ((double)rand() / RAND_MAX - .5) * 2 * sigma; // Simulated measurement with noise

		// Predict step
		lkf_predict(&kf);

		// Update with the simulated measurement
		lkf_update(&kf, &z);
		
		// Print the state estimate
		fprintf(file, "%d\t%f\t%f\t%f\t%f\t%f\n", i, sin(dt*i), z, kf.x[0], cos((double)i / 100.), kf.x[1]);

	}
	fclose(file);

	// Clean up
	lkf_destroy(&kf);
	
	return EXIT_SUCCESS;
}