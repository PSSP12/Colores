#ifndef BMP_UTILS_H
#define BMP_UTILS_H

#include <stdint.h> // for uintX_t
#include <stdio.h>  // for FILE*
#include <stdlib.h> // for malloc, free

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)

// BMP File Header (14 bytes)
typedef struct {
    uint16_t fileType;     // File type, always "BM" (0x4D42)
    uint32_t fileSize;     // Size of the file (in bytes)
    uint16_t reserved1;    // Reserved, always 0
    uint16_t reserved2;    // Reserved, always 0
    uint32_t offsetData;   // Start position of pixel data (in bytes)
} BMPFileHeader;

// BMP Info Header (40 bytes)
typedef struct {
    uint32_t size;            // Size of the info header (40 bytes)
    int32_t width;            // Width of the image
    int32_t height;           // Height of the image (can be negative)
    uint16_t planes;          // Number of color planes, must be 1
    uint16_t bitCount;        // Number of bits per pixel (24 for RGB)
    uint32_t compression;     // Compression type (0 for no compression)
    uint32_t sizeImage;       // Image size (may be 0 for uncompressed)
    int32_t xPelsPerMeter;    // Pixels per meter along X-axis
    int32_t yPelsPerMeter;    // Pixels per meter along Y-axis
    uint32_t colorsUsed;      // Number of colors used (0 for all)
    uint32_t colorsImportant; // Number of important colors (0 = all)
} BMPInfoHeader;

#pragma pack(pop)

/**
 * Loads a rectangular section of a BMP image.
 * 
 * @param filename Path to the BMP file
 * @param startX X-coordinate of the top-left corner
 * @param startY Y-coordinate of the top-left corner
 * @param sectionWidth Width of the section
 * @param sectionHeight Height of the section
 * @return Pointer to raw RGB pixel data (must be freed by caller), or NULL on error
 */
unsigned char* load_bmp_section(const char* filename, int startX, int startY, int sectionWidth, int sectionHeight);

/**
 * Searches for a specific color within a region of a BMP file.
 *
 * @param filename Path to the BMP file
 * @param X1 Top-left X of the search region
 * @param Y1 Top-left Y of the search region
 * @param X2 Bottom-right X of the search region
 * @param Y2 Bottom-right Y of the search region
 * @param margin Permitted margin for color components (tolerance)
 * @param Color Target RGB color (3 bytes: R, G, B)
 * @param X Output array for X-coordinates of matching pixels
 * @param Y Output array for Y-coordinates of matching pixels
 * @return 0 on success, non-zero on failure
 */
int search_colors(const char* filename, int X1, int Y1, int X2, int Y2, unsigned char margin, unsigned char* Color, int* X, int* Y);

/**
 * Searches for multiple colors in a BMP region.
 *
 * @param filename Path to the BMP file
 * @param X1 Top-left X of the region
 * @param Y1 Top-left Y of the region
 * @param X2 Bottom-right X of the region
 * @param Y2 Bottom-right Y of the region
 * @param threshold Allowed difference from target colors (tolerance)
 * @param Colors Array of RGB color triplets (length = num * 3)
 * @param num Number of color triplets
 * @param X Output array of X coordinates for all matches (size = num * N)
 * @param Y Output array of Y coordinates for all matches (size = num * N)
 * @return 0 on success
 */
int search_colors_V2(const char* filename, int X1, int Y1, int X2, int Y2, unsigned char threshold, unsigned char* Colors, int num, int* X, int* Y);

/**
 * Finds pairs of points with a specified distance within a margin.
 *
 * @param X1 X-coordinates of first point set (null-terminated with 0)
 * @param Y1 Y-coordinates of first point set (null-terminated with 0)
 * @param X2 X-coordinates of second point set (null-terminated with 0)
 * @param Y2 Y-coordinates of second point set (null-terminated with 0)
 * @param dist Target distance
 * @param margin Allowed deviation from target distance
 * @param Xres1 Output X-coordinates from set 1
 * @param Yres1 Output Y-coordinates from set 1
 * @param Xres2 Output X-coordinates from set 2
 * @param Yres2 Output Y-coordinates from set 2
 * @return 0 on success
 */
int search_distances(int* X1, int* Y1, int* X2, int* Y2, float dist, float margin, int* Xres1, int* Yres1, int* Xres2, int* Yres2);

/**
 * Finds pairs of points with a specified angle within a margin.
 *
 * @param X1 X-coordinates of first point set (null-terminated with 0)
 * @param Y1 Y-coordinates of first point set (null-terminated with 0)
 * @param X2 X-coordinates of second point set (null-terminated with 0)
 * @param Y2 Y-coordinates of second point set (null-terminated with 0)
 * @param num Number of pairs to check
 * @param margin Allowed deviation from target angle
 * @param Xres1 Output X-coordinates from set 1
 * @param Yres1 Output Y-coordinates from set 1
 * @param Xres2 Output X-coordinates from set 2
 * @param Yres2 Output Y-coordinates from set 2
 * @return 0 on success
 */
int search_angles(int* X1, int* Y1, int* X2, int* Y2, float ang, float margin, int* Xres1, int* Yres1, int* Xres2, int* Yres2);

int search_angles_optimized(int* X1, int* Y1, int* X2, int* Y2, float ang, float margin, int* Xres1, int* Yres1, int* Xres2, int* Yres2);

#ifdef __cplusplus
}
#endif

#endif // BMP_UTILS_H
