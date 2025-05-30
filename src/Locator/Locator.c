/**
 * @file Locator.c
 * @author Pablo Sánchez
 * @brief This file implements a simple locator for a robot using two motors.
 * @version 0.1
 * @date 2025-05-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
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

/* // TODO list:
1.  Maybe change the integration method for better resolution
2.  Test for optimal number of flushes to file
*/

#define Sleep( msec ) usleep(( msec ) * 1000 ) // Sleeps for msec milliseconds

float per = 0.196;          // Wheel perimeter in m | 2*pi*Radius
volatile int Ports[2];      // Ports variable
pthread_t Locator_thread;   // Global thread

/**
 * @brief Locator thread function.
 * 
 * @param arg Pointer to the argument (not used).
 * @return int Returns 0 on success.
 */
static int Locator(void* arg){
    
    struct timespec T1, T2;      // Time storage
    int i;                      // Loop index
    float   Speed_R, Speed_L,   // Wheel's speed (calculations)
            Speed_G;            // Turning speed
    int LSR, LSL;               // Wheel's speed (reading)
    float ang, dt, T, d;        // Calculus variables
    uint8_t Big_R, Big_L;       // Motors
    float r[2];                 // Position vector
    uint8_t flush_step;         // Space between flushes

    // Basic initializations
    r[0] = 0; r[1] = 0;
    ev3_init();
    ev3_tacho_init();
    ev3_search_tacho_plugged_in(Ports[0], EXT_PORT__NONE_, &Big_R, 0);
    ev3_search_tacho_plugged_in(Ports[1], EXT_PORT__NONE_, &Big_L, 0);
    ang = 0.0;
    T = 0;
    d = 0.15;
    flush_step = 100;   //! Could cause errors when accessing the data from main thread
    i = 0;

    // Logging
    FILE *file;
    file = fopen("locator_data.csv", "w");
    fprintf(file, "dt\tX\tY\tang\n");
    
    clock_gettime(CLOCK_MONOTONIC, &T1);
    
    // Main loop
    while(true) {
        get_tacho_speed(Big_R, &LSR);
        get_tacho_speed(Big_L, &LSL);
        Speed_L = (float)LSL;           // Speed reading
        Speed_R = (float)LSR;
        Speed_L = Speed_L*per/360.0;    // Actual speed
        Speed_R = Speed_R*per/360.0;
        Speed_G = (Speed_L+Speed_R)/2;  // Turning speed
        clock_gettime(CLOCK_MONOTONIC, &T2);
        dt = (T2.tv_sec - T1.tv_sec) + (T2.tv_nsec - T1.tv_nsec) * 1e-9; // Time difference in seconds
        clock_gettime(CLOCK_MONOTONIC, &T1);
        T += dt;                        // Total time
        ang += ((Speed_L-Speed_R)/d)*dt;// Current angle
        r[0] += dt*Speed_G*cos(ang);    // Update position
        r[1] += dt*Speed_G*sin(ang);
        fprintf(file, "%f\t%f\t%f\t%f\n", dt, r[0], r[1], ang);     // Logs into file
        if (i%flush_step == 0){fflush(file);}   // Actually save into memory
        i += 1;
    }
    fclose(file);
    return 0;
}

/**
 * @brief Starts the locator thread with specified ports.
 * 
 * @param Port1 The port for the right motor.
 * @param Port2 The port for the left motor.
 * @return int Returns 0 on success.
 */
int start_Locator(int Port1, int Port2){
    Ports[0] = Port1; Ports[1] = Port2;
    pthread_create(&Locator_thread, NULL, Locator, NULL);   // Starts the main thread
}

/**
 * @brief Stops the locator thread.
 * 
 * @return int Returns 0 on success.
 */
int stop(void) {
    pthread_cancel(Locator_thread);     // Stops the thread
    pthread_join(Locator_thread, NULL); // Waits for the thread to reach stopping point
    return 0;
}
