#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#pragma pack(push, 1)
// BMP File Header (14 bytes)
typedef struct {
    uint16_t fileType;   // File type, always "BM" (0x4D42)
    uint32_t fileSize;   // Size of the file (in bytes)
    uint16_t reserved1;  // Reserved, always 0
    uint16_t reserved2;  // Reserved, always 0
    uint32_t offsetData; // Start position of pixel data (in bytes)
} BMPFileHeader;

// BMP Info Header (40 bytes)
typedef struct {
    uint32_t size;         // Size of the info header (40 bytes)
    int32_t width;         // Width of the image
    int32_t height;        // Height of the image (can be negative)
    uint16_t planes;       // Number of color planes, must be 1
    uint16_t bitCount;     // Number of bits per pixel (24 for RGB)
    uint32_t compression;  // Compression type (0 for no compression)
    uint32_t sizeImage;    // Image size (may be 0 for uncompressed images)
    int32_t xPelsPerMeter; // Pixels per meter along X-axis
    int32_t yPelsPerMeter; // Pixels per meter along Y-axis
    uint32_t colorsUsed;   // Number of colors used (0 for all colors)
    uint32_t colorsImportant; // Number of important colors (0 = all)
} BMPInfoHeader;
#pragma pack(pop)

// Function to load a section of BMP
unsigned char* load_bmp_section(const char* filename, int startX, int startY, int sectionWidth, int sectionHeight) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Could not open BMP file %s\n", filename);
        return NULL;
    }

    // Read the file and info headers
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    fread(&fileHeader, sizeof(BMPFileHeader), 1, file);
    fread(&infoHeader, sizeof(BMPInfoHeader), 1, file);

    // Verify the BMP file type and bit depth (must be 24-bit RGB)
    if (fileHeader.fileType != 0x4D42 || infoHeader.bitCount != 24) {
        fprintf(stderr, "Error: Unsupported BMP format (must be 24-bit BMP)\n");
        fclose(file);
        return NULL;
    }

    int width = infoHeader.width;
    int height = infoHeader.height;

    // Validate that the section is within the image bounds
    if (startX < 0 || startY < 0 || startX + sectionWidth > width || startY + sectionHeight > height) {
        fprintf(stderr, "Error: Requested section is out of image bounds.\n");
        fclose(file);
        return NULL;
    }

    // BMP rows are padded to a multiple of 4 bytes, so calculate the padded row size
    int row_padded = (width * 3 + 3) & (~3);

    // Allocate memory for the section (no padding)
    unsigned char* section = (unsigned char*)malloc(sectionWidth * sectionHeight * 3);

    if (!section) {
        perror("Error allocating memory for section");
        fclose(file);
        return NULL;
    }

    // Read from bottom-left to top-left
    for (int row = 0; row < sectionHeight; ++row) {
        int filePos = fileHeader.offsetData + (height - (startY + row + 1)) * row_padded + startX * 3;
        fseek(file, filePos, SEEK_SET);

        // Read the section row into the buffer (no padding for this)
        fread(&section[row * sectionWidth * 3], sizeof(unsigned char), sectionWidth * 3, file);
    }

    fclose(file);
    return section;
}

// Function to search for a color inside an image section
int search_colors(const char* filename, int X1, int Y1, int X2, int Y2, unsigned char margin, unsigned char* Color, int* X, int* Y) {
    // Searches for the colors 'Color' in a box of vertices ('X1', 'Y1') and ('X2', 'Y2'), with a margin of error for the color 'margin'.
    // The section will be taken from 'filename' and the coordinates in the image of the found colors will be put inside 'X' and 'Y'.
    int sectionWidth = X2-X1;
    int sectionHeight = Y2-Y1;
    
    unsigned char ColorUpper[3];
    unsigned char ColorLower[3];

    // Adjust margins to not step out of bounds
    if (255 - Color[0] < margin) {ColorUpper[0] = 255;} else {ColorUpper[0] = Color[0] + margin;}
    if (255 - Color[1] < margin) {ColorUpper[1] = 255;} else {ColorUpper[1] = Color[1] + margin;}
    if (255 - Color[2] < margin) {ColorUpper[2] = 255;} else {ColorUpper[2] = Color[2] + margin;}
    
    if (Color[0] < margin) {ColorLower[0] = 0;} else {ColorLower[0] = Color[0] - margin;}
    if (Color[1] < margin) {ColorLower[1] = 0;} else {ColorLower[1] = Color[1] - margin;}
    if (Color[2] < margin) {ColorLower[2] = 0;} else {ColorLower[2] = Color[2] - margin;}

    // Load the section
    unsigned char* section = load_bmp_section(filename, X1, Y1, sectionWidth, sectionHeight);
    if (!section) {
        perror("Couldn't load the section");
    }

    int* Pos;
    int o = 0;
    Pos = (int*)calloc(sectionWidth*sectionHeight, sizeof(int));
    Pos[0] = -1;

    // Search for all the pixels in the section inside the margins
    for (int i = 0; i < sectionHeight*sectionWidth*3; i += 3) {
        if (section[i] <= ColorUpper[0] && section[i] >= ColorLower[0]) {
            if (section[i+1] <= ColorUpper[1] && section[i+1] >= ColorLower[1]) {
                if (section[i+2] <= ColorUpper[2] && section[i+2] >= ColorLower[2]) {
                    Pos[o] = i/3;
                    o++;
                }
            }
        }
    }

    // Convert Pos array to X and Y coordinates
    if (Pos[0] != -1) {
        int o = 1;
        int rem = 0;
        rem = Pos[0]%(X2-X1);
        X[0] = rem + X1 + 1;
        Y[0] = (Pos[0]-rem) / (X2-X1) + Y1 + 1;

        while (Pos[o] != 0) {
            rem = Pos[o]%(X2-X1);
            X[o] = rem + X1 + 1;
            Y[o] = (Pos[o]-rem) / (X2-X1) + Y1 + 1;
            o++;
        }
    }
    
    free(Pos); // Free the allocated memory
    free(section);
    return 0;
}

// Improved color search function
//! Testing needed. Supposedly can search for however many colors in the same section. Should be faster and more memory efficient
int search_colors_V2(const char* filename, int X1, int Y1, int X2, int Y2, unsigned char threshold, unsigned char* Colors, int num, int* X, int* Y) {
    // Searches for the given colors in the Colors variable among the square of vertices (X1, Y1) (X2, Y2).

    int sectionWidth = X2-X1;
    int sectionHeight = Y2-Y1;
    int N = sectionHeight*sectionWidth;

    // Load the section
    unsigned char* section = load_bmp_section(filename, X1, Y1, sectionWidth, sectionHeight);
    if (!section) {
        perror("Couldn't load the section");
    }

    unsigned char Color[3];
    int* Pos = (int*)calloc(sectionWidth*sectionHeight, sizeof(int));
    unsigned char ColorUpper[3];
    unsigned char ColorLower[3];

    for (int n = 0; n < num; n++) {
        Color[0] = Colors[n*3];
        Color[1] = Colors[n*3+1];
        Color[2] = Colors[n*3+1];
        
        // Adjust margins to not step out of bounds
        if (255 - Color[0] < threshold) {ColorUpper[0] = 255;} else {ColorUpper[0] = Color[0] + threshold;}
        if (255 - Color[1] < threshold) {ColorUpper[1] = 255;} else {ColorUpper[1] = Color[1] + threshold;}
        if (255 - Color[2] < threshold) {ColorUpper[2] = 255;} else {ColorUpper[2] = Color[2] + threshold;}
        
        if (Color[0] < threshold) {ColorLower[0] = 0;} else {ColorLower[0] = Color[0] - threshold;}
        if (Color[1] < threshold) {ColorLower[1] = 0;} else {ColorLower[1] = Color[1] - threshold;}
        if (Color[2] < threshold) {ColorLower[2] = 0;} else {ColorLower[2] = Color[2] - threshold;}

        int o = 0;
        Pos[0] = -1; // Ensure we have at least one element

        // Search for all the pixels in the section inside the margins
        for (int i = 0; i < sectionHeight*sectionWidth*3; i += 3) {
            if (section[i] <= ColorUpper[0] && section[i] >= ColorLower[0]) {
                if (section[i+1] <= ColorUpper[1] && section[i+1] >= ColorLower[1]) {
                    if (section[i+2] <= ColorUpper[2] && section[i+2] >= ColorLower[2]) {
                        Pos[o] = i/3;
                        o++;
                    }
                }
            }
        }

        // Convert Pos array to X and Y coordinates
        if (Pos[0] != -1) {
            int o = 1;
            int rem = 0;
            rem = Pos[0]%(X2-X1);
            X[0+n*N] = rem + X1 + 1;
            Y[0+n*N] = (Pos[0]-rem) / (X2-X1) + Y1 + 1;

            while (Pos[o] != 0) {
                rem = Pos[o]%(X2-X1);
                X[o+n*N] = rem + X1 + 1;
                Y[o+n*N] = (Pos[o]-rem) / (X2-X1) + Y1 + 1;
                o++;
            }
        }
        
    }

    free(Pos); // Free the allocated memory
    free(section);
    return 0;

}

// Function to search for fixed distances betwwen points
int search_distances(int* X1, int* Y1, int* X2, int* Y2, float dist, float margin, int* Xres1, int* Yres1, int* Xres2, int* Yres2) {
    int X1s = 0;
    int X2s = 0;
    // Count number of points
    while (X1[X1s] != 0) {X1s++;}
    while (X2[X2s] != 0) {X2s++;}

    int o = 0;
    float temp_dist = 0;
    for (int i = 0; i < X1s; i++) {
        // printf("%i\t\\\t%i\n", i, X1s);
        for (int p = 0; p < X2s; p++) {
            temp_dist = sqrt((X2[p]-X1[i])*(X2[p]-X1[i]) + (Y2[p]-Y1[i])*(Y2[p]-Y1[i]));
            if (temp_dist-margin <= dist && temp_dist+margin >= dist) {
                Xres1[o] = X1[i]; Xres2[o] = X2[p];
                Yres1[o] = Y1[i]; Yres2[o] = Y2[p];
                o++;
            }
        }
    }

    return 0;
}

int main(void) {
    int X1 = 0; int Y1 = 0;
    int X2 = 100; int Y2 = 100;
    int margin = 20;
    FILE *file;
    file = fopen("ImageData.csv", "w");
    fprintf(file, "Xi\tYi\tXd\tYd\n");
    unsigned char ColorI[3] = {0, 0, 0};
    unsigned char ColorD[3] = {0, 0, 0};

    int* XI = (int*)calloc((X2-X1)*(Y2-Y1), sizeof(int));
    int* YI = (int*)calloc((X2-X1)*(Y2-Y1), sizeof(int));
    int* XD = (int*)calloc((X2-X1)*(Y2-Y1), sizeof(int));
    int* YD = (int*)calloc((X2-X1)*(Y2-Y1), sizeof(int));

    search_colors("../Image/MasterpieceMat.bmp", X1, Y1, X2, Y2, margin, ColorI, XI, YI);
    search_colors("../Image/MasterpieceMat.bmp", X1, Y1, X2, Y2, margin, ColorD, XD, YD);

    int I, D;
    I = 0; D = 0;
    while (XI[I] != 0) {I++;}
    while (XD[D] != 0) {D++;}

    int* XIr = (int*)calloc(I*D, sizeof(int));
    int* YIr = (int*)calloc(I*D, sizeof(int));
    int* XDr = (int*)calloc(I*D, sizeof(int));
    int* YDr = (int*)calloc(I*D, sizeof(int));

    printf("Started searching the distances\n");
    search_distances(XI, YI, XD, YD, 10, 10, XIr, YIr, XDr, YDr);
    printf("Done searching for distances\n");
    free(XI); free(YI); free(XD); free(YD);

    int i = 0;
    while (XIr[i] != 0) {
        fprintf(file, "%i\t%i\t%i\t%i\n", XIr[i], YIr[i], XDr[i], YDr[i]);
        i++;
    }
    free(XIr); free(YIr); free(XDr); free(YDr);
    

    fclose(file);
}