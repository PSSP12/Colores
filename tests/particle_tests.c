#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "PFP.h"

#define NUM_PARTICLES 20
#define N_STEPS 10

int main() {
    // --- Load the floor image ---
    BMPImage *floor = read_bmp("assets/Images/MasterpieceMat.bmp");
    if (!floor) {
        printf("Failed to load floor image.\n");
        return 1;
    }

    // --- Define sensor colors ---
    unsigned char color_left[3]  = {0, 0, 0};   // example BGR color
    unsigned char color_right[3] = {255, 255, 255};

    // --- Initialize particles ---
    Particle *particles = (Particle*)malloc(NUM_PARTICLES * sizeof(Particle));
    for (int i = 0; i < NUM_PARTICLES; i++) {
        particles[i].x_left  = rand() % floor->width;
        particles[i].y_left  = rand() % floor->height;
        particles[i].x_right = rand() % floor->width;
        particles[i].y_right = rand() % floor->height;
    }

    // --- Open CSV file to store results ---
    FILE *out = fopen("tests/TestingData/pf_results.csv", "w");
    fprintf(out, "step,particle,x_left,y_left,x_right,y_right,dt\n");

    // --- Run particle filter for multiple steps ---
    for (int step = 0; step < N_STEPS; step++) {
        clock_t t0 = clock();

        particle_filter_step(floor, color_left, color_right,
                             NUM_PARTICLES, particles,
                             20.0f, 5.0f,    // dist_fixed, dist_margin
                             30.0f, 5.0f);   // angle_fixed, angle_margin

        clock_t t1 = clock();
        double dt = (double)(t1 - t0) / CLOCKS_PER_SEC;

        // --- Store particle positions ---
        for (int i = 0; i < NUM_PARTICLES; i++) {
            fprintf(out, "%d,%d,%f,%f,%f,%f,%f\n",
                    step, i,
                    particles[i].x_left, particles[i].y_left,
                    particles[i].x_right, particles[i].y_right,
                    dt);
        }
    }

    fclose(out);
    free(particles);
    free_bmp(floor);

    printf("Particle filter test complete. Results saved to pf_results.csv\n");
    return 0;
}
