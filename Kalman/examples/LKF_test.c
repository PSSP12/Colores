#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include "ev3.h"
#include "ev3_light.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"
#include "../Filters/LKF.c"
#include <sys/sysinfo.h>
#include <unistd.h>

#define Sleep( msec ) usleep(( msec ) * 1000 ) // Función para pausas de un número de milisegundos

// Sensors
#define GYRO_IZDA INPUT_1
#define GYRO_DCHA INPUT_2
#define COL_IZDA INPUT_3
#define COL_DCHA INPUT_4

// Motors
#define MED_IZDA OUTPUT_D
#define MED_DCHA OUTPUT_C
#define GRANDE_I OUTPUT_A
#define GRANDE_D OUTPUT_B

int main(void) {

    while(ev3_init() < 1){Sleep(10);}
    while(ev3_sensor_init() < 1){Sleep(10);}

    uint8_t Gyro_I, Gyro_D, Color_I, Color_D, Grande_I, Grande_D, Mediano_I, Mediano_D;
    float ang, rate, T;
    float both[2];
    ang = 0; rate = 0;
    ev3_search_sensor_plugged_in(GYRO_DCHA, EXT_PORT__NONE_, &Gyro_D, 0);
    ev3_search_sensor_plugged_in(GYRO_IZDA, EXT_PORT__NONE_, &Gyro_I, 0);

    // Size of observation and state
    int state_size = 2;
    int obs_size = 1;

    // File to store the test data
    FILE *file;
    file = fopen("data.csv", "w");

    // File header
    fprintf(file, "Time\tA_Kalman\tS_Kalman\tA_Raw\tS_Raw\n");

    // Initialize filter and allocate memory
    KalmanFilter kf;
    kf.x = (double *)malloc(state_size * sizeof(double));               // State vector
    kf.P = (double *)malloc(state_size * state_size * sizeof(double));  // Covariance matrix
    kf.F = (double *)malloc(state_size * state_size * sizeof(double));  // Transition matrix (model)
    kf.Q = (double *)malloc(state_size * state_size * sizeof(double));  // Process noise matrix
    kf.H = (double *)malloc(obs_size * state_size * sizeof(double));    // Observation matrix
    kf.R = (double *)malloc(obs_size * obs_size * sizeof(double));      // Observation noise matrix

    struct timeval T1, T2;
    float dt;
    T = 0;

    // Initialize filter variables
    double xx[2][1] = {
        {0},
        {0}
    }; memcpy(kf.x, xx, state_size*sizeof(double));

    double Pp[2][2] = { // Auxiliar variable | Commodity
        {1, 0},
        {0, 1}
    }; memcpy(kf.P, Pp, state_size*state_size*sizeof(double));    // Actual assignation of values

    double Ff[2][2] = {
        {1, .1},
        {0, 1}
    }; memcpy(kf.F, Ff, state_size*state_size*sizeof(double));

    double Qq[2][2] = {
        {.1, 0},
        {0, .1}
    }; memcpy(kf.Q, Qq, state_size*state_size*sizeof(double));

    double Hh[1][2] = {
        {0, 1}
    }; memcpy(kf.H, Hh, obs_size*state_size*sizeof(double));

    double Rr[1][1] = {
        {.01}
    }; memcpy(kf.R, Rr, obs_size*obs_size*sizeof(double));

    double z[] = {0};

    initialize(obs_size, state_size);

    set_sensor_mode(Gyro_D, "GYRO-G&A");
    set_sensor_mode(Gyro_I, "GYRO-ANG");

    // Main loop

    gettimeofday(&T1, NULL);
    for (int t = 0; t < 10000; ++t) {
        gettimeofday(&T2, NULL);
        dt = (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec)/1000000.0; // Time difference in microseconds
        T += dt;  // Convert microseconds to seconds and update total time
        gettimeofday(&T1, NULL);  // Update T1 for the next iteration

        // printf("Iteration %d\tValue: %lf\tTime: %lf\n", t, z[0], (T2.tv_usec - T1.tv_usec)/1000000.0); // Debugging printer

        get_sensor_value0(Gyro_I, &ang);

        get_sensor_value0(Gyro_D, &both[0]);
        get_sensor_value1(Gyro_D, &both[1]);

        z[0] = both[1];

        double Ff[2][2] = {
            {1, dt},  // Convert dt to seconds
            {0, 1}
        };
        memcpy(kf.F, Ff, state_size * state_size * sizeof(double));


        kalman_predict(&kf, state_size);
        kalman_update(&kf, z, state_size, obs_size);

        fprintf(file, "%f\t%f\t%f\t%f\t%f\n", T, kf.x[0], kf.x[1], both[0], both[1]);
    }
    fclose(file);

    finalize();

    // Liberar la memoria
    free(kf.x);
    free(kf.P);
    free(kf.F);
    free(kf.Q);
    free(kf.H);
    free(kf.R);

    return 0;
}
