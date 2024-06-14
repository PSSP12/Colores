// Author Pablo Sánchez de Pablos

// Control system for a robot to run following a certain angle

// NOTES:
// Gyro modes - Text associated:
// GYRO-ANG | GYRO-RATE | GYRO-FAS | GYRO-G&A | TILT-ANG | TILT-RATE

#include <stdio.h>
#include <sys/time.h>
#include "ev3.h"
#include "ev3_light.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"
// #include "coroutine.h"

// UNIX
#include <unistd.h>
#define Sleep( msec ) usleep(( msec ) * 1000 ) // Función para pausas de un número de milisegundos

// Settings del robot Cambiar en función del robot

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

int Trazador(float degs, float P0, float ang)
{
    // Inicialización
    while(ev3_init() < 1){Sleep(10);}
    while(ev3_sensor_init() < 1){Sleep(10);}
    while(ev3_tacho_init() < 1){Sleep(10);}

    char* g_mode = "GYRO-ANG";
    
    uint8_t Gyro_I, Gyro_D, Color_I, Color_D, Grande_I, Grande_D, Mediano_I, Mediano_D;
    
    ev3_search_sensor_plugged_in(GYRO_DCHA, EXT_PORT__NONE_, &Gyro_D, 0);
    ev3_search_sensor_plugged_in(GYRO_IZDA, EXT_PORT__NONE_, &Gyro_I, 0);
    set_sensor_mode(Gyro_D, g_mode);
    set_sensor_mode(Gyro_I, g_mode);

    // ev3_search_sensor_plugged_in(COL_DCHA, EXT_PORT__NONE_, &Color_D, 0);
    // ev3_search_sensor_plugged_in(COL_IZDA, EXT_PORT__NONE_, &Color_I, 0);

    // ev3_search_tacho_plugged_in(MED_DCHA, EXT_PORT__NONE_, Mediano_D, 0); // Posiblemente quite los medianos | No son necesarios
    // ev3_search_tacho_plugged_in(MED_IZDA, EXT_PORT__NONE_, Mediano_I, 0);
    ev3_search_tacho_plugged_in(GRANDE_D, EXT_PORT__NONE_, &Grande_D, 0);
    ev3_search_tacho_plugged_in(GRANDE_I, EXT_PORT__NONE_, &Grande_I, 0);

    float ang_d, ang_i;
    float ang_mean, err, err0, err_i, err_d;
    struct timeval T1, T2;
    float dt;
    int rot_d, rot_i;
    int init_d, init_i;
    float P = 10;
    float I = 0;
    float D = 1;
    // char* comm = "TACHO_RUN_FOREVER";
    
    get_tacho_position(Grande_D, &init_d);
    get_tacho_position(Grande_I, &init_i);
    rot_d = init_d; rot_i = init_i;

    // Settings | Tengo que ver como de utiles son
    // set_tacho_time_sp(Grande_D, 5000);
    // set_tacho_time_sp(Grande_I, 5000);
    // set_tacho_ramp_up_sp(Grande_D, 2000);
    // set_tacho_ramp_up_sp(Grande_I, 2000);
    // set_tacho_ramp_down_sp(Grande_D, 2000);
    // set_tacho_ramp_down_sp(Grande_I, 2000);

    // printf("Posición inicial: %d\n", rot_d+rot_i);

    set_tacho_speed_sp(Grande_D, P0);
    set_tacho_speed_sp(Grande_I, P0);
    set_tacho_command_inx(Grande_D, TACHO_RUN_FOREVER);
    set_tacho_command_inx(Grande_I, TACHO_RUN_FOREVER);
    err0 = 0;
    // T2 = clock();
    gettimeofday(&T1, NULL);

    while((rot_d+rot_i-init_d-init_i)*(rot_d+rot_i-init_d-init_i) < 4*degs*degs){
        
        get_tacho_position(Grande_D, &rot_d);
        get_tacho_position(Grande_I, &rot_i);
        // printf("Posición: %d\n", rot_d+rot_i);
        get_sensor_value0(Gyro_D, &ang_d);
        get_sensor_value0(Gyro_I, &ang_i);
        ang_mean = (ang_d+ang_i)/2;
        // printf("%f\n", ang_mean); // Debugging

        err = ang-ang_mean;
        // T1 = clock();
        gettimeofday(&T2, NULL);
        dt = (T2.tv_usec-T1.tv_usec)/1000000;
        err_d = (err-err0)/(T2.tv_usec-T1.tv_usec);
        err_i = err_i+(T2.tv_usec-T1.tv_usec)*err;
        // printf("%i\n", (T2.tv_usec-T1.tv_usec));
        // T2 = clock();
        gettimeofday(&T1, NULL);
        set_tacho_speed_sp(Grande_D, P0 + P*err + I*err_i + D*err_d);
        set_tacho_speed_sp(Grande_I, P0 - (P*err + I*err_i + D*err_d));
        set_tacho_command_inx(Grande_D, TACHO_RUN_FOREVER);
        set_tacho_command_inx(Grande_I, TACHO_RUN_FOREVER);
        err0 = err;
    }
    set_tacho_command_inx(Grande_D, TACHO_STOP);
    set_tacho_command_inx(Grande_I, TACHO_STOP);

    ev3_uninit();
}