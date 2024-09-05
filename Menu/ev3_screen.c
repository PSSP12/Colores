#include "ev3.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>

#define SCREEN_WIDTH 178
#define SCREEN_HEIGHT 128
#define BYTES_PER_PIXEL 4  // 32-bit color depth (4 bytes per pixel)

void set_pixel(uint32_t* buffer, int x, int y, uint32_t color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        int pixel_index = x + y * SCREEN_WIDTH;
        buffer[pixel_index] = color;
    }
}

void draw_square(uint32_t* buffer, int x, int y, int size, uint32_t color) {
    for (int i = 0; i < size; ++i) {
        set_pixel(buffer, x + i, y, color);               // Top edge
        set_pixel(buffer, x + i, y + size - 1, color);    // Bottom edge
        set_pixel(buffer, x, y + i, color);               // Left edge
        set_pixel(buffer, x + size - 1, y + i, color);    // Right edge
    }
}

void clear_screen(uint32_t* buffer, int num_pixels, uint32_t color) {
    for (int i = 0; i < num_pixels; ++i) {
        buffer[i] = color;
    }
}