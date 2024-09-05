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
#include "ev3_screen.c"

int main() {
    system("sudo systemctl stop brickman"); // Turn brickman off

    if (ev3_init() == -1) {
        fprintf(stderr, "Error initializing EV3\n");
        return 1;
    }

    int fb = open("/dev/fb0", O_RDWR);
    if (fb == -1) {
        perror("Error opening framebuffer");
        ev3_uninit();
        return 1;
    }

    struct fb_var_screeninfo vinfo;
    if (ioctl(fb, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("Error reading screen info");
        close(fb);
        ev3_uninit();
        return 1;
    }

    int screen_size = vinfo.xres_virtual * vinfo.yres_virtual * vinfo.bits_per_pixel / 8;
    int num_pixels = screen_size / BYTES_PER_PIXEL;

    uint32_t* framebuffer = (uint32_t*)mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
    if (framebuffer == MAP_FAILED) {
        perror("Error mapping framebuffer");
        close(fb);
        ev3_uninit();
        return 1;
    }

    clear_screen(framebuffer, num_pixels, 0xFFFFFFFF);  // Fill with black color (0xFF000000 for 32-bit ARGB)
    draw_square(framebuffer, 40, 30, 50, 0x00000000);  // Draw a red square (0xFFFF0000 for 32-bit ARGB)

    if (munmap(framebuffer, screen_size)) {
        perror("Error unmapping framebuffer");
    }

    close(fb);
    ev3_uninit();
    system("sudo systemctl start brickman"); // Turn brickman on

    return 0;
}
