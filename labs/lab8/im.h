#ifndef IM_H
#define IM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "eyebot.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// Kernel convolution functions
void Convolve(BYTE *imageIn, int width, int height, BYTE *imageOut, const int mask[]);
void HoG(BYTE *imageIn, int width, int height, BYTE* gradients, BYTE* angles);

// Resizing functions
void BilinearDownscale(BYTE* imgIn, int w1, int h1, BYTE* imgOut, int w2, int h2, int channels);
void DownscaleImage(BYTE* imgIn, int inWidth, int inHeight, BYTE* imgOut, int outWidth, int outHeight);

// Read/Write functions
void WritePNM(BYTE* imgOut, int width, int height, char* filename, int channels);
int ReadPNM(char *filename, BYTE* img);

#endif