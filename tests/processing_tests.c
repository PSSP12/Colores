#include <Processing.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <math.h>
#include <time.h>

int main(void) {
    int X1 = 150; int Y1 = 0;
    int X2 = 300; int Y2 = 200;
    int sectionWidth = X2-X1;
    int sectionHeight = Y2-Y1;
    int N = sectionHeight*sectionWidth;
    unsigned char ColorI[3] = {0, 0, 0};
    // unsigned char ColorD[3] = {200, 48, 61};
    unsigned char ColorD[3] = {44, 44, 44};
    // unsigned char ColorT[6] = {0, 0, 0, 200, 48, 61};
    unsigned char ColorT[6] = {0, 0, 0, 254, 208, 146}; // Left Right
    unsigned char color_margin = 20;
    float angle = M_PI_2;
	float ang_margin = 0.1;
    float dist = 120.;
	float dist_margin = 10.1;
    int num = 2;
    FILE *file;

	// Create “TestingData” if it doesn’t already exist:
	if (mkdir("tests/TestingData", 0755) != 0) {
		// EEXIST is fine; any other error we bail out
		if (errno != EEXIST) {
			perror("mkdir TestingData");
			return EXIT_FAILURE;
		}
	}

    file = fopen("tests/TestingData/ImageData.csv", "w");
	fprintf(file, "X1=%i\tY1=%i\tX2=%i\tY2=%i\tColorI=[%i, %i, %i]\tColorD=[%i, %i, %i]\tcolor_margin=%i\tangle=%f\tang_margin=%f\tdist=%f\tdist_margin=%f\n", X1, Y1, X2, Y2, ColorI[0], ColorI[1], ColorI[2], ColorD[0], ColorD[1], ColorD[2], color_margin, angle, ang_margin, dist, dist_margin);
    fprintf(file, "Xi\tYi\tXd\tYd\n");

    // int* XI = (int*)calloc((X2-X1)*(Y2-Y1), sizeof(int));
    // int* YI = (int*)calloc((X2-X1)*(Y2-Y1), sizeof(int));
    // int* XD = (int*)calloc((X2-X1)*(Y2-Y1), sizeof(int));
    // int* YD = (int*)calloc((X2-X1)*(Y2-Y1), sizeof(int));
    int* XT = (int*)calloc(num*(X2-X1)*(Y2-Y1), sizeof(int));
    int* YT = (int*)calloc(num*(X2-X1)*(Y2-Y1), sizeof(int));

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
	printf("Started searching for colors\n");
    // search_colors_V2("assets/Images/MasterpieceMat.bmp", X1, Y1, X2, Y2, color_margin, ColorI, 1, XI, YI);
    // search_colors_V2("assets/Images/MasterpieceMat.bmp", X1, Y1, X2, Y2, color_margin, ColorD, 1, XD, YD);
    search_colors_V2("assets/Images/MasterpieceMat.bmp", X1, Y1, X2, Y2, color_margin, ColorT, num, XT, YT);
	printf("Done searching for colors\n");

    int* XI = XT; int* YI = YT;
    int* XD = XT + N; int* YD = YT + N;

    int I, D;
    I = 0; D = 0;
    while (XI[I] != 0) {I++;}
    while (XD[D] != 0) {D++;}

    int* XIr = (int*)calloc(I*D, sizeof(int));
    int* YIr = (int*)calloc(I*D, sizeof(int));
    int* XDr = (int*)calloc(I*D, sizeof(int));
    int* YDr = (int*)calloc(I*D, sizeof(int));

    int* XIrr = (int*)calloc(I*D, sizeof(int));
    int* YIrr = (int*)calloc(I*D, sizeof(int));
    int* XDrr = (int*)calloc(I*D, sizeof(int));
    int* YDrr = (int*)calloc(I*D, sizeof(int));

    printf("Started searching the distances\n");
    search_distances(XI, YI, XD, YD, dist, dist_margin, XIr, YIr, XDr, YDr);
    printf("Done searching for distances\n");
    printf("Started searching the angles\n");
    search_angles_optimized(XIr, YIr, XDr, YDr, angle, ang_margin, XIrr, YIrr, XDrr, YDrr);
    printf("Done searching for angles\n");

    clock_gettime(CLOCK_MONOTONIC, &end);
    long seconds = end.tv_sec - start.tv_sec;
    long nanoseconds = end.tv_nsec - start.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;
    printf("Time taken: %.6f seconds\n", elapsed);

    int i = 0;
    while (XIrr[i] != 0) {
        fprintf(file, "%i\t%i\t%i\t%i\n", XIrr[i], YIrr[i], XDrr[i], YDrr[i]);
        // fprintf(file, "%i\t%i\t%i\t%i\n", XIr[i], YIr[i], XDr[i], YDr[i]);
        // fprintf(file, "%i\t%i\t%i\t%i\n", XI[i], YI[i], XD[i], YD[i]);
        i++;
    }
    free(XT); free(YT);
    // free(XI); free(YI); free(XD); free(YD);
    free(XIr); free(YIr); free(XDr); free(YDr);
    free(XIrr); free(YIrr); free(XDrr); free(YDrr);
    
    fclose(file);

	return 0;
}