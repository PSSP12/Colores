#include <stdio.h>
#include <sys/time.h>
#include "ev3.h"
#include "ev3_light.h"
#include "ev3_port.h"
#include "ev3_sensor.h"
#include "ev3_tacho.h"
#include <math.h>
#include <unistd.h>
#include <pthread.h>

#define Sleep( msec ) usleep(( msec ) * 1000 ) // Sleeps for msec milliseconds

float per = 0.196;          // Wheel perimeter in m | 2*pi*Radius
volatile int Ports[2];      // Ports variable
pthread_t Locator_thread;   // Global thread

void* Locator(void* arg){
    struct timeval T1, T2;
    int i;
    float Speed_R, Speed_L, Speed_G;
    int LSR, LSL;
    float ang, dt, T, d;
    uint8_t Big_R, Big_L;
    float r[2];
    r[0] = 0; r[1] = 0;
    ev3_init();
    ev3_tacho_init();
    ev3_search_tacho_plugged_in(Ports[0], EXT_PORT__NONE_, &Big_R, 0);
    ev3_search_tacho_plugged_in(Ports[1], EXT_PORT__NONE_, &Big_L, 0);
    ang = 0.0;
    T = 0;
    d = 0.15;

    FILE *file;
    file = fopen("locator_data.csv", "w");
    fprintf(file, "dt\tX\tY\tang\n");
    
    gettimeofday(&T1, NULL);
    while(true) {
        get_tacho_speed(Big_R, &LSR);
        get_tacho_speed(Big_L, &LSL);
        Speed_L = (float)LSL;
        Speed_R = (float)LSR;
        Speed_L = Speed_L*per/360.0;
        Speed_R = Speed_R*per/360.0;
        Speed_G = (Speed_L+Speed_R)/2;
        gettimeofday(&T2, NULL);
        dt = (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec)/1000000.0; // Time difference in seconds
        gettimeofday(&T1, NULL);
        T += dt;
        ang += ((Speed_L-Speed_R)/d)*dt;
        // printf("%f\t%f\n", Speed_R, ang);    // Debugging
        fflush(stdout);
        r[0] += dt*Speed_G*cos(ang);
        r[1] += dt*Speed_G*sin(ang);
        fprintf(file, "%f\t%f\t%f\t%f\n", dt, r[0], r[1], ang);     // Logs into file
        fflush(file);
    }
    fclose(file);
    return 0;
}

int start_Locator(int Port1, int Port2){
    Ports[0] = Port1; Ports[1] = Port2;
    pthread_create(&Locator_thread, NULL, Locator, NULL);   // Starts the main thread
}

int stop(void) {
    pthread_cancel(Locator_thread);     // Stops the thread
    pthread_join(Locator_thread, NULL); // Waits for the thread to reach stopping point
    return 0;
}
