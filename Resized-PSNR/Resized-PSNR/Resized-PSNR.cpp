// Resized-psnr.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
	int width = -1;
	int height = -1;
	int num_frame = -1;
	long long CountFrame;
	int form = 0;
	int ColorSpace = 1;
	double psnr_sphere_Y = 0;
	double psnr_sphere_U = 0;
	double psnr_sphere_V = 0;

	char *Comp_File1 = (char *)malloc(sizeof(char) * 1000);
	char *Comp_File2 = (char *)malloc(sizeof(char) * 1000);

	FILE *FpOriginalFile;
	FILE *FpReconFile;

	// ========== patch processing  ==========  
	int iArgIdx = 1;
	if (argc > 1) {
		if (strcmp("-help", argv[iArgIdx]) == 0) {
			printf("\n");
			printf("-w		width\n");
			printf("-h		height\n");
			printf("-n		number of frames to be compared\n");
			printf("-f		format of the video\n");
			printf("-x		image color space\n");
			printf("-o		original file\n");
			printf("-r		reconstructed file\n");
			//system("pause");
			exit(0);
		}
	}

	for (iArgIdx = 1; iArgIdx < argc; iArgIdx += 2) {
		if (strcmp("-w", argv[iArgIdx]) == 0)  width = atoi(argv[iArgIdx + 1]);
		else if (strcmp("-h", argv[iArgIdx]) == 0)  height = atoi(argv[iArgIdx + 1]);
		else if (strcmp("-n", argv[iArgIdx]) == 0)  num_frame = atoi(argv[iArgIdx + 1]);
		else if (strcmp("-f", argv[iArgIdx]) == 0)  form = atoi(argv[iArgIdx + 1]);
		else if (strcmp("-x", argv[iArgIdx]) == 0)  ColorSpace = atoi(argv[iArgIdx + 1]);
		else if (strcmp("-o", argv[iArgIdx]) == 0)  strcpy(Comp_File1, argv[iArgIdx + 1]);
		else if (strcmp("-r", argv[iArgIdx]) == 0)  strcpy(Comp_File2, argv[iArgIdx + 1]);

	}

	//==========  check and parse the input parameters  ==========  
	if (width == -1 || height == -1) {
		printf("please input the resolution of the video \n");
		//system("pause");
		exit(1);
	}

	if (num_frame == -1) {
		printf("please input the number of frames to be compared \n");
		//system("pause");
		exit(1);
	}

	if (Comp_File1 == NULL || Comp_File2 == NULL) {
		printf("please input the comparative file \n");
		//system("pause");
		exit(1);
	}

	if (form == 0)	  printf(" Format : Equirectangular \n");
	/*else if (form == 1)	  printf(" Format : Cubic with 3*2 type\n");
	else if (form == 2)	  printf(" Format : Cubic with 2*3 type\n");
	else if (form == 3)	  printf(" Format : Cubic with 6*1 type\n");
	else if (form == 4)   printf(" Format : Cubic with 1*6 type\n");
	else if (form == 5)   printf(" Format : Cubic with T type\n");
	else if (form == 6)   printf(" Format : Icosahedron \n");
	else if (form == 7)   printf(" Format : Octahedron \n");*/
	else {
		printf("Invalid input option for format index \n");
		//system("pause");
		exit(1);
	}

	printf(" Resolution: %d * %d\n", width, height);
	printf(" number of frames: %d\n\n", num_frame);
	printf(" Frame  Resized-PSNR_Y  Resized-PSNR_U  Resized-PSNR_V\n");
	if (ColorSpace == 1) {
		unsigned char *OriginalFileBuffer;
		unsigned char *ReconFileBuffer;
		OriginalFileBuffer = (unsigned char *)malloc(sizeof(unsigned char)*width*height * 3 / 2);
		ReconFileBuffer = (unsigned char *)malloc(sizeof(unsigned char)*width*height * 3 / 2);

		for (CountFrame = 0; CountFrame < num_frame; CountFrame++) {

			printf("   %d ", CountFrame);
			FpOriginalFile = fopen(Comp_File1, "rb");

			long long offset = height*width * 3 / 2 * CountFrame;

			_fseeki64(FpOriginalFile, offset, SEEK_SET);
			if (fread(OriginalFileBuffer, height*width * 3 / 2, 1, FpOriginalFile) != 1) {
				printf("can't open the file %s \n", Comp_File1);
				//system("pause");
				exit(1);
			}
			fclose(FpOriginalFile);

			FpReconFile = fopen(Comp_File2, "rb");
			_fseeki64(FpReconFile, offset, SEEK_SET);
			if (fread(ReconFileBuffer, height*width * 3 / 2, 1, FpReconFile) != 1) {
				printf("can't open the file %s \n", Comp_File2);
				//system("pause");
				exit(1);
			}
			fclose(FpReconFile);

			double* thetaYi = (double *)malloc(sizeof(double)*height);
			int* PointPerlineY = (int *)malloc(sizeof(int)*height);
			double* RePointPositonY = (double *)malloc(sizeof(double)*width);
			double* thetaUVi = (double *)malloc(sizeof(double)*height/2);
			int* PointPerlineUV = (int *)malloc(sizeof(int)*height/2);
			double* RePointPositonUV = (double *)malloc(sizeof(double)*width/2);
			int N;
			int i,j;
			double d;
			float p, q;
			int size_y = width * height;
			int size_uv = size_y >> 2;
			N = height;
			double diff_y = 0, diff_u = 0, diff_v = 0;
			for (i = 0; i < height; i++){
				thetaYi[i] = abs(i - (N / 2 - 0.5)) *3.14159265/ N;
				PointPerlineY[i] = round(2 * N*cos(thetaYi[i]) / cos(3.14159265 / (2 * N)));
			}
			
			for (j = 0; j < height; j++) {
				for (i = 0; i < PointPerlineY[j]; i++) {
					RePointPositonY[i] = i*(width) / (double)(PointPerlineY[j]) + 0.5*((width) / (double)(PointPerlineY[j])-1);
					p = 1 - (RePointPositonY[i] - floor(RePointPositonY[i]));
					q = 1 - p;
					if (q == 0)
						d = OriginalFileBuffer[j * width + (int)floor(RePointPositonY[i])] - ReconFileBuffer[j * width + (int)floor(RePointPositonY[i])];
					else
						d = p*OriginalFileBuffer[j * width + (int)floor(RePointPositonY[i])] + q*OriginalFileBuffer[j * width + (int)floor(RePointPositonY[i]) + 1] - p*ReconFileBuffer[j * width + (int)floor(RePointPositonY[i])] - q*ReconFileBuffer[j * width + (int)floor(RePointPositonY[i]) + 1];
					diff_y += d * d;
				}
			}
			size_y = 0;
			for (i = 0; i < height; i++) {
				size_y += PointPerlineY[i];
			}
			double psrR_Y = (float)(10 * log10(65025.0f * size_y / diff_y));
			printf("    %.4f", psrR_Y);
			psnr_sphere_Y += psrR_Y;

			N = height/2;
			for (i = 0; i < height/2; i++){
				thetaUVi[i] = abs(i - (N / 2 - 0.5)) *3.14159265 / N;
				PointPerlineUV[i] = round(2 * N*cos(thetaUVi[i]) / cos(3.14159265 / (2 * N)));
			}
			for (j = 0; j < height / 2; j++) {
				for (i = 0; i < PointPerlineUV[j]; i++) {
					RePointPositonUV[i] = i*(width) / (double)(PointPerlineUV[j]) + 0.5*((width) / (double)(PointPerlineUV[j]) - 1);//点的位置映射
					p = 1 - (RePointPositonUV[i] - floor(RePointPositonUV[i]));
					q = 1 - p;
					if (q == 0)
						d = OriginalFileBuffer[width*height + j*width / 2 + (int)floor(RePointPositonUV[i])] - ReconFileBuffer[width*height + j*width / 2 + (int)floor(RePointPositonUV[i])];
					else
						d = p*OriginalFileBuffer[width*height + j*width / 2 + (int)floor(RePointPositonUV[i])] + q*OriginalFileBuffer[width*height + j*width / 2 + (int)floor(RePointPositonUV[i]) + 1] - p*ReconFileBuffer[width*height + j*width / 2 + (int)floor(RePointPositonUV[i])] - q*ReconFileBuffer[width*height + j*width / 2 + (int)floor(RePointPositonUV[i]) + 1];//经纬图双线性插值到正弦映射
					diff_u += d * d;
				}
			}
			size_uv = 0;
			for (i = 0; i < height/2; i++) {
				size_uv += PointPerlineUV[i];
			}
			double psrR_U = (float)(10 * log10(65025.0f * size_uv / diff_u));

			printf("    %.4f", psrR_U);
			psnr_sphere_U += psrR_U;

			for (j = 0; j < height / 2; j++) {
				for (i = 0; i < PointPerlineUV[j]; i++) {
					RePointPositonUV[i] = i*(width) / (double)(PointPerlineUV[j]) + 0.5*((width) / (double)(PointPerlineUV[j]) - 1);
					p = 1 - (RePointPositonUV[i] - floor(RePointPositonUV[i]));
					q = 1 - p;
					if (q == 0)
						d = OriginalFileBuffer[width*height * 5 / 4 + j*width / 2 + (int)floor(RePointPositonUV[i])] - ReconFileBuffer[width*height * 5 / 4 + j*width / 2 + (int)floor(RePointPositonUV[i])];
					else
						d = p*OriginalFileBuffer[width*height * 5 / 4 + j*width / 2 + (int)floor(RePointPositonUV[i])] + q*OriginalFileBuffer[width*height * 5 / 4 + j*width / 2 + (int)floor(RePointPositonUV[i])] - p*ReconFileBuffer[width*height * 5 / 4 + j*width / 2 + (int)floor(RePointPositonUV[i])] - q*ReconFileBuffer[width*height * 5 / 4 + j*width / 2 + (int)floor(RePointPositonUV[i])];
					diff_v += d * d;
				}
			}
			double psrR_V = (float)(10 * log10(65025.0f * size_uv / diff_v));

			printf("    %.4f\n", psrR_V);
			psnr_sphere_V += psrR_V;
		}

	}
	else {
		printf("Invalid input option for color space \n");
		//system("pause");
		exit(1);
	}

	psnr_sphere_Y = psnr_sphere_Y / num_frame;
	psnr_sphere_U = psnr_sphere_U / num_frame;
	psnr_sphere_V = psnr_sphere_V / num_frame;

	printf("\n  Average R-PSNR_Y = %.4f \n", psnr_sphere_Y);
	printf("  Average R-PSNR_U = %.4f \n", psnr_sphere_U);
	printf("  Average R-PSNR_V = %.4f \n\n\n", psnr_sphere_V);

	//system("pause");
}


