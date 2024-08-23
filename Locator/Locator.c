#include <stdio.h>
#include <sys/time.h>
#include "ev3.h"
#include "ev3_light.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"
#include <math.h>

float per = 0.196; // Wheel perimeter in m | 2*pi*Radius

int main(void){
    
    struct timeval T1, T2;
    int i;
    float Speed_R, Speed_L, Speed_G;
    int LSR, LSL;
    float ang, dt, T, d;
    uint8_t Big_R, Big_L;
    float r[2];
    ev3_init();
    ev3_tacho_init();
    ev3_search_tacho_plugged_in(OUTPUT_B, EXT_PORT__NONE_, &Big_R, 0);
    ev3_search_tacho_plugged_in(OUTPUT_A, EXT_PORT__NONE_, &Big_L, 0);
    set_tacho_speed_sp(Big_R, 100);
    set_tacho_speed_sp(Big_L, 50);
    set_tacho_command_inx(Big_R, TACHO_RUN_FOREVER);
    set_tacho_command_inx(Big_L, TACHO_RUN_FOREVER);
    ang = 0.0;
    T = 0;
    d = .15;

    FILE *file;
    file = fopen("locator_data.csv", "w");
    fprintf(file, "X\tY\n");
    
    gettimeofday(&T1, NULL);
    for(i=0; i<10000; i++) {
        get_tacho_speed(Big_R, &LSR);
        get_tacho_speed(Big_L, &LSL);
        Speed_L = (float)LSL;
        Speed_R = (float)LSR;
        Speed_L = Speed_L*per/360.0;
        Speed_R = Speed_R*per/360.0;
        Speed_G = (Speed_L+Speed_R)/2;
        gettimeofday(&T2, NULL);
        dt = (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec)/1000000.0; // Time difference in microseconds
        gettimeofday(&T1, NULL);
        T += dt;  // Convert microseconds to seconds and update total time
        // ang += Speed_G*dt/((d/2)*(Speed_L+Speed_R)/(Speed_L-Speed_R)); // Pablo
        // printf("%f\t%f\n", Speed_G*dt/((d/2)*(Speed_L+Speed_R)/(Speed_L-Speed_R)), ang);
        ang += ((Speed_L-Speed_R)/d)*dt; // ChatGPT
        printf("%f\t%f\n", (Speed_L-Speed_R)/d, ang);
        r[0] += dt*Speed_G*cos(ang);
        r[1] += dt*Speed_G*sin(ang);
        fprintf(file, "%f\t%f\n", r[0], r[1]);
    }
    set_tacho_command_inx(Big_R, TACHO_STOP);
}