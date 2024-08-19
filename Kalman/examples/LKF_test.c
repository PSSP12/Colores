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

// Sensores:
#define GYRO_IZDA INPUT_1
#define GYRO_DCHA INPUT_2
#define COL_IZDA INPUT_3
#define COL_DCHA INPUT_4

// Motores
#define MED_IZDA OUTPUT_D
#define MED_DCHA OUTPUT_C
#define GRANDE_I OUTPUT_A
#define GRANDE_D OUTPUT_B

int main(void) {

    while(ev3_init() < 1){Sleep(10);}
    while(ev3_sensor_init() < 1){Sleep(10);}

    uint8_t Gyro_I, Gyro_D, Color_I, Color_D, Grande_I, Grande_D, Mediano_I, Mediano_D;
    float ang_d;
    ev3_search_sensor_plugged_in(GYRO_DCHA, EXT_PORT__NONE_, &Gyro_D, 0);

    // Tamaño de los estados y las observaciones
    int state_size = 2;  // Matrices de tamaño 4x4
    int obs_size = 1;    // Matrices de tamaño 2x4 (para 2 observaciones)
    FILE *file;
    file = fopen("data.txt", "w");

    // Inicializar el filtro de Kalman
    KalmanFilter kf;
    kf.x = (double *)malloc(state_size * sizeof(double));  // Vector de estados
    kf.P = (double *)malloc(state_size * state_size * sizeof(double));  // Matriz de covarianza
    kf.F = (double *)malloc(state_size * state_size * sizeof(double));  // Matriz de transición
    kf.Q = (double *)malloc(state_size * state_size * sizeof(double));  // Matriz de ruido del proceso
    kf.H = (double *)malloc(obs_size * state_size * sizeof(double));  // Matriz de observación
    kf.R = (double *)malloc(obs_size * obs_size * sizeof(double));  // Matriz de ruido de observación

    double dt = .1;

    // Aquí deberías inicializar kf.x, kf.P, kf.F, kf.Q, kf.H, y kf.R con valores apropiados
    double xx[2][1] = {
        {1},
        {0}
    }; memcpy(kf.x, xx, state_size*sizeof(double));

    double Pp[2][2] = { // Auxiliar variable | Commodity
        {1, 0},
        {0, 1}
    }; memcpy(kf.P, Pp, state_size*state_size*sizeof(double));    // Actual assignation of values

    double Ff[2][2] = {
        {1, dt},
        {0, 1}
    }; memcpy(kf.F, Ff, state_size*state_size*sizeof(double));

    double Qq[2][2] = {
        {1, 0},
        {0, 1}
    }; memcpy(kf.Q, Qq, state_size*state_size*sizeof(double));

    double Hh[1][2] = {
        {1, 0}
    }; memcpy(kf.H, Hh, obs_size*state_size*sizeof(double));

    double Rr[1][1] = {
        {1}
    }; memcpy(kf.R, Rr, obs_size*obs_size*sizeof(double));

    // Ejemplo de una observación
    double z[] = {1.0};  // Observación de ejemplo (vector de tamaño 2)

    initialize(obs_size, state_size);

    printf("%f\n", kf.x[0]);

    // Bucle de predicción y actualización
    struct sysinfo mem_info;
    for (int t = 0; t < 1000; ++t) {
        printf("Iteración %d\tValor: %lf\n", t, kf.x[0]);
        
        get_sensor_value0(Gyro_D, &ang_d);
        z[0] = ang_d;

        // Retrieve memory information
        sysinfo(&mem_info);
        // Print memory information
        printf("Total RAM: %lu MB\n", mem_info.totalram / 1024 / 1024);
        printf("Free RAM: %lu MB\n", mem_info.freeram / 1024 / 1024);
        printf("Used RAM: %lu MB\n", (mem_info.totalram - mem_info.freeram) / 1024 / 1024);

        kalman_predict(&kf, state_size); //! Problem here
        kalman_update(&kf, z, state_size, obs_size);
        fprintf(file, "%f\n", kf.x[0]);
        
        // Aquí puedes imprimir o utilizar el estado estimado kf.x
    }
    fclose(file);

    // finalize();

    // Liberar la memoria
    free(kf.x);
    free(kf.P);
    free(kf.F);
    free(kf.Q);
    free(kf.H);
    free(kf.R);

    return 0;
}

