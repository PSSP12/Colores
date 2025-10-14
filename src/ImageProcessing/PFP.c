#include "PFP.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// --- BMP Reading ---
BMPImage* read_bmp(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;

    BMPFileHeader header;
    BMPInfoHeader info;

	if (fread(&header, sizeof(header), 1, f) != 1) {
		fclose(f);
		return NULL;
	}
    if (fread(&info, sizeof(info), 1, f) != 1) {
		fclose(f);
		return NULL;
	}

    if (header.fileType != 0x4D42 || info.bitCount != 24) {
        fclose(f);
        return NULL;
    }

    int row_padded = (info.width*3 + 3) & (~3);
    unsigned char* pixels = malloc(row_padded * info.height);
    if (!pixels) { fclose(f); return NULL; }

    fseek(f, header.offsetData, SEEK_SET);
    for (int y = 0; y < info.height; y++) {
        if (fread(pixels + y*row_padded, 1, row_padded, f) != row_padded) {
			free(pixels);
			fclose(f);
			return NULL;
		}
    }
    fclose(f);

    BMPImage* img = malloc(sizeof(BMPImage));
    img->width = info.width;
    img->height = info.height;
    img->row_stride = row_padded;
    img->pixels = pixels;
    return img;
}

void free_bmp(BMPImage* img) {
    if (!img) return;
    free(img->pixels);
    free(img);
}

// --- Color Search ---
int search_color_section(BMPImage* img, int X1, int Y1, int X2, int Y2,
                         unsigned char* Color, unsigned char margin,
                         int* X, int* Y, int max_results) {
    int o = 0;
    int row_stride = img->row_stride;

    unsigned char cu[3] = {
        (Color[0]+margin>255)?255:Color[0]+margin,
        (Color[1]+margin>255)?255:Color[1]+margin,
        (Color[2]+margin>255)?255:Color[2]+margin
    };
    unsigned char cl[3] = {
        (Color[0]<margin)?0:Color[0]-margin,
        (Color[1]<margin)?0:Color[1]-margin,
        (Color[2]<margin)?0:Color[2]-margin
    };

    for (int y=Y1; y<Y2 && o<max_results; y++) {
        for (int x=X1; x<X2 && o<max_results; x++) {
            unsigned char* px = img->pixels + (img->height-1-y)*row_stride + x*3;
            if (px[2] >= cl[0] && px[2] <= cu[0] &&
                px[1] >= cl[1] && px[1] <= cu[1] &&
                px[0] >= cl[2] && px[0] <= cu[2]) {
                X[o] = x;
                Y[o] = y;
                o++;
            }
        }
    }
    return o;
}

// --- Distance / Angle Checks ---
int check_distances(int* X1, int* Y1, int* X2, int* Y2, int n1, int n2,
                    float target_dist, float margin,
                    int* Xres1, int* Yres1, int* Xres2, int* Yres2) {
    int o=0;
    float min_d2 = (target_dist-margin)*(target_dist-margin);
    float max_d2 = (target_dist+margin)*(target_dist+margin);

    for (int i=0;i<n1;i++)
        for (int j=0;j<n2;j++){
            float dx = X2[j]-X1[i];
            float dy = Y2[j]-Y1[i];
            float d2 = dx*dx + dy*dy;
            if (d2>=min_d2 && d2<=max_d2){
                Xres1[o]=X1[i]; Yres1[o]=Y1[i];
                Xres2[o]=X2[j]; Yres2[o]=Y2[j];
                o++;
            }
        }
    return o;
}

int check_angles(int* X1, int* Y1, int* X2, int* Y2, int n1, int n2,
                 float target_ang, float margin,
                 int* Xres1, int* Yres1, int* Xres2, int* Yres2) {
    int o=0;
    float c_ang = cos(target_ang);
    float s_ang = sin(target_ang);
    float cos_margin = cos(margin);

    for(int i=0;i<n1;i++){
        for(int j=0;j<n2;j++){
            float dx = X2[j]-X1[i];
            float dy = Y2[j]-Y1[i];
            float norm = sqrt(dx*dx+dy*dy);
            if (norm==0) continue;
            float dot = (dx*c_ang + dy*s_ang)/norm;
            if (dot>=cos_margin){
                Xres1[o]=X1[i]; Yres1[o]=Y1[i];
                Xres2[o]=X2[j]; Yres2[o]=Y2[j];
                o++;
            }
        }
    }
    return o;
}

// --- Particle Filter Step ---
void particle_filter_step(BMPImage* img,
                          unsigned char* color_left,
                          unsigned char* color_right,
                          int num_particles,
                          Particle* particles,
                          float dist_fixed,
                          float dist_margin,
                          float angle_fixed,
                          float angle_margin) {
    int max_results = 1000;
    int *Xl = malloc(max_results*sizeof(int));
    int *Yl = malloc(max_results*sizeof(int));
    int *Xr = malloc(max_results*sizeof(int));
    int *Yr = malloc(max_results*sizeof(int));

    // For simplicity: use whole image
    int nL = search_color_section(img, 0, 0, img->width, img->height,
                                  color_left, 20, Xl, Yl, max_results);
    int nR = search_color_section(img, 0, 0, img->width, img->height,
                                  color_right, 20, Xr, Yr, max_results);

    int *Xd1 = malloc(nL*nR*sizeof(int));
    int *Yd1 = malloc(nL*nR*sizeof(int));
    int *Xd2 = malloc(nL*nR*sizeof(int));
    int *Yd2 = malloc(nL*nR*sizeof(int));

    int count = check_distances(Xl,Yl,Xr,Yr,nL,nR,dist_fixed,dist_margin,
                                Xd1,Yd1,Xd2,Yd2);

    // Filter angles
    int *Xa1 = malloc(count*sizeof(int));
    int *Ya1 = malloc(count*sizeof(int));
    int *Xa2 = malloc(count*sizeof(int));
    int *Ya2 = malloc(count*sizeof(int));

    int n_final = check_angles(Xd1,Yd1,Xd2,Yd2,count,count,angle_fixed,angle_margin,
                               Xa1,Ya1,Xa2,Ya2);

    // Update particles randomly among valid matches
    for(int i=0;i<num_particles;i++){
        if(n_final>0){
            int k = rand()%n_final;
            particles[i].x_left = Xa1[k];
            particles[i].y_left = Ya1[k];
            particles[i].x_right = Xa2[k];
            particles[i].y_right = Ya2[k];
        }
    }

    free(Xl); free(Yl); free(Xr); free(Yr);
    free(Xd1); free(Yd1); free(Xd2); free(Yd2);
    free(Xa1); free(Ya1); free(Xa2); free(Ya2);
}
