#ifndef PFP_H
#define PFP_H

#include <stdint.h>

// BMP headers
#pragma pack(push, 1)
typedef struct {
    uint16_t fileType;   
    uint32_t fileSize;   
    uint16_t reserved1;  
    uint16_t reserved2;  
    uint32_t offsetData; 
} BMPFileHeader;

typedef struct {
    uint32_t size;         
    int32_t width;         
    int32_t height;        
    uint16_t planes;       
    uint16_t bitCount;     
    uint32_t compression;  
    uint32_t sizeImage;    
    int32_t xPelsPerMeter; 
    int32_t yPelsPerMeter; 
    uint32_t colorsUsed;   
    uint32_t colorsImportant;
} BMPInfoHeader;
#pragma pack(pop)

// BMP image container
typedef struct {
    int width;
    int height;
    int row_stride;
    unsigned char *pixels;
} BMPImage;

// Load entire BMP
BMPImage* read_bmp(const char* filename);

// Free BMP
void free_bmp(BMPImage* img);

// Search for color in BMP section
int search_color_section(BMPImage* img, int X1, int Y1, int X2, int Y2,
                         unsigned char* Color, unsigned char margin,
                         int* X, int* Y, int max_results);

// Check distances between points
int check_distances(int* X1, int* Y1, int* X2, int* Y2, int n1, int n2,
                    float target_dist, float margin,
                    int* Xres1, int* Yres1, int* Xres2, int* Yres2);

// Check angles between points
int check_angles(int* X1, int* Y1, int* X2, int* Y2, int n1, int n2,
                 float target_ang, float margin,
                 int* Xres1, int* Yres1, int* Xres2, int* Yres2);

// Particle filter for two-color sensors
typedef struct {
    float x_left, y_left;
    float x_right, y_right;
} Particle;

void particle_filter_step(BMPImage* img,
                          unsigned char* color_left,
                          unsigned char* color_right,
                          int num_particles,
                          Particle* particles,
                          float dist_fixed,
                          float dist_margin,
                          float angle_fixed,
                          float angle_margin);

#endif
