#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "ev3.h"
#include "ev3_light.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"
#include "coroutine.h"
// #include "../utils/matrix.c"
#include "../Filters/LKF.c"

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
    // Inicialización
    while(ev3_init() < 1){Sleep(10);}
    while(ev3_sensor_init() < 1){Sleep(10);}
    while(ev3_tacho_init() < 1){Sleep(10);}

    uint8_t Gyro_I, Gyro_D, Color_I, Color_D, Grande_I, Grande_D, Mediano_I, Mediano_D;
    float ang_d;

    ev3_search_sensor_plugged_in(GYRO_DCHA, EXT_PORT__NONE_, &Gyro_D, 0);

    get_sensor_value0(Gyro_D, &ang_d);
    printf("%f\n", ang_d);

    double A[3][3] = {
        {1, 2, 3},
        {4, 3, 2},
        {9, 7, 5}
    };
    int size[2] = {3, 3};

    printer((int*)&size, (double*)&A);
}