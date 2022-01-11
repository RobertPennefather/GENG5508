#include "im.h"

#define white 255
#define black 0

/*
*   Kernel Convolution
*/

// Convole a 3x3 mask
// Index of mask:
// | 0 1 2 |
// | 3 4 5 |
// | 6 7 8 |

void Convolve(BYTE *imageIn, int width, int height, BYTE *imageOut, const int mask[])
{   
    int value = 0;
    int sum = 0;
    int i;

    memset(imageOut, 0, width); // clear first row
    for(int j = 0; j < 9; j++)
        sum += abs(mask[j]);

    for (i = width; i < (height-1)*width; i++)
    {  value = imageIn[i-width-1] * mask[0] + imageIn[i-width] * mask[1] + imageIn[i-width+1] * mask[2] +
                imageIn[i-1] * mask[3] + imageIn[i] * mask[4] + imageIn[i+1] * mask[5] + 
                imageIn[i+width-1] * mask[6] + imageIn[i+width] * mask[7] + imageIn[i+width+1] * mask[8];
                
        value = abs(value)/3;
        if (value > white) value = white;
        imageOut[i] = (BYTE) value;
    }

    memset(imageOut + i, 0, width); // clear last row
}

void HoG(BYTE *imageIn, int width, int height, BYTE* gradients, BYTE* angles)
{   
    int dX, dY, i;
    float grad, angle;

    memset(gradients, 0, width); // clear first row
    memset(angles, 0, width); // clear first row

    for (i = width; i < (height-1)*width; i++)
    {
        dX = -imageIn[i-1] + imageIn[i+1];
        dY = -imageIn[i-width] + imageIn[i+width];
        
        grad = sqrt(dX*dX + dY*dY);
        gradients[i] = grad > white ? white : (BYTE) grad;
        if(dX != 0)
        {
            angle = (atan(dY/dX) * 180.0 / M_PI);
            angles[i] = (BYTE) (angle < 0 ? angle + 180 : angle);    
        } 
        else
            angles[i] = 90;
    }

    memset(gradients + i, 0, width); // clear last row
    memset(angles + i, 0, width); // clear last row
}

/*
*   Resize Images
*/
// 160 x 120 -> 80 x 60 -> 10 * 6
void BilinearDownscale(BYTE* imgIn, int w1, int h1, BYTE* imgOut, int w2, int h2, int channels)
{
    int a, b, c, d, x, y, index;
    float value;
    float xRatio = (float) (w1/w2);
    float yRatio = (float) (h1/h2);
    float xDiff, yDiff;
    int offset = 0;

    for(int i = 0; i < h2; i++)
    {
        for(int j = 0; j < w2; j++)
        {   
            x = (int) (xRatio * j);
            y = (int) (yRatio * i);
            xDiff = (xRatio * j) - x;
            yDiff = (yRatio * i) - y;
            index = (y*w1 + x);

            // Array indices
            a = index * channels;
            b = (index + 1) * channels;
            c = (index + w1) * channels;
            d = (index + w1 + 1) * channels;

            for(int k = 0; k < channels; k++)
            {
                value = imgIn[a + k] * (1-xDiff) * (1-yDiff) + 
                    imgIn[b + k] * xDiff * (1-yDiff) + 
                    imgIn[c + k] * yDiff * (1-xDiff) +
                    imgIn[d + k] * xDiff * yDiff;
                imgOut[offset++] = (BYTE) value;
            }
        }
    }
}

// Downscales an image
void DownscaleImage(BYTE* imgIn, int inWidth, int inHeight, BYTE* imgOut, int outWidth, int outHeight)
{
    int height = inHeight;
    int width = inWidth;
    BYTE *currImage = imgIn;
    int iteration = 0;

    while(height % 2 == 0 && width % 2 == 0)
    {
        int halfWidth = width/2;
        int halfHeight = height/2;
        BYTE* downImage = malloc(halfWidth * halfHeight * 3);
        if(downImage == NULL)
        {
            fprintf(stderr, "malloc failed\n");
            return;
        }
        for(int i = 0; i < height; i+=2)
        {
            for(int j = 0; j < width; j+=2)
            {
                for(int k = 0; k < 3; k++)
                {
                    downImage[3*(halfWidth*(i/2) + (j/2)) + k] = (currImage[3 * (width*i + j)] + 
                        currImage[3*(width*i + j + 1) + k] + 
                        currImage[3*(width*(i+1) + j)+k] + 
                        currImage[3*(width*(i+1) + (j+1)) + k])/4;
                }
            }
        }
        width = halfWidth;
        height = halfHeight;

        if(iteration > 0)
            free(currImage);
        currImage = downImage;
        iteration++;
    }

    BilinearDownscale(currImage, width, height, imgOut, outWidth, outHeight, 3);
}

/*
*   Reading and Writing PNM Files
*/

// Read a PBM, PGM, or PPM File
int ReadPNM(char *filename, BYTE* img)  // Read PNM file, fill/crop if req.
// Returns: 1 =PBM, 2 =PGM, 3 =PPM (or 4, 5, 6 for binary versions)
{   char next, code[100];
    int  codecount[4] = {0, 1, 1, 3};  /* 1 byte for PBM, PGM, 3 bytes for PPM */
    int  codenum, ascii, width, height, levels, count = 0;
    int  maxheight, maxwidth, num, last;
    int IM_WIDTH, IM_HEIGHT;
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) { printf("!error: read file\n"); return -1;}

    fscanf(fp, "%s", code);
    codenum = (int) (code[1] - '0'); // make into an integer
    printf("image code %d\n",codenum);
    ascii = codenum<4;  /* P1,2,3 are ASCII, P4,5,6 are binary */
    if (!ascii) codenum = codenum - 3;  //reduce to 1..3
    if (codenum<1 || codenum>3) { printf("bad image code\n"); return -1;}
    do next = fgetc(fp);  // clear white space
    while (next=='\n' || next==' ');

    while (next=='#')  /* comment until end of line */
    { do
        { printf("%c", next);
            next = fgetc(fp);
        } while (next !='\n');
        printf("\n");
        next = fgetc(fp);
    }
    ungetc(next, fp);  // give character back
    fscanf(fp, "%d %d", &width, &height);
    if (codenum>1) fscanf(fp, "%d", &levels);
    printf("Read %s file %s: W%d H%d\n", code, filename, width, height);
    // Don't fill extra spaces
    IM_WIDTH = width;
    IM_HEIGHT = height;
    do next = fgetc(fp);  // clear white space
    while (next=='\n' || next==' ');
    ungetc(next, fp);  // give character back

    if (codenum==1 && ascii) // PBM ASCII
    { for (int i=0; i<height; i++)
            for (int j=0; j<width; j++)
            { fscanf(fp, "%d", &num);
                if (i<IM_HEIGHT && j<IM_WIDTH)  // read image
                { if (num) img[count++] = 0; else img[count++] = 255; }
            }
        for (int j=width; j<IM_WIDTH; j++) img[count++] = 0; // fill line end

        for (int i=height; i<IM_HEIGHT; i++)
            for (int j=0; j<IM_HEIGHT; j++)
                img[count++] = 0; // fill remaining lines
    }

    else if (codenum==1 && !ascii) // PBM BINARY
    { for (int i=0; i<height; i++)
        { for (int j=0; j<width; j+=8)
            { if (i<IM_HEIGHT && j<IM_WIDTH)  // read image
                { num = fgetc(fp);
                    if (j+8 < width) last = 8; else last = width % 8;
                    for (int l=0; l<last; l++)
                        if (num & (0x80 >> l)) img[count++] = 0; else img[count++] = 255;
                }
                else fgetc(fp);  // read pixels beyond parameter size
            }
        }
    }


    else // PGM (gray), PPM (color)
    { maxwidth  = MAX(width,  IM_WIDTH);
        maxheight = MAX(height, IM_HEIGHT);
        for (int i=0; i<maxheight; i++)
        { for (int j=0; j<maxwidth; j++)
                for (int k=0; k<codecount[codenum]; k++)
                { if (i<MIN(IM_HEIGHT,height) && j<MIN(IM_WIDTH,width))  // read image
                    { if (ascii) fscanf(fp, "%d", (int *) &img[count++]);
                        else img[count++] = fgetc(fp);
                    }
                    else if (j>=IM_WIDTH || i>=IM_HEIGHT) fgetc(fp);  // read pixels beyond parameter size
                    else if (j>=width || i>=height) img[count++] = 0;  // fill remaining area with black pixels
                }
        }
    }
    fclose(fp);
    return codenum; // (P) 1..3 fpr PBM, PGM, PPM
}

// Write a Grayscale or RGB image to PPM or PGM
// Expects maximum value to be 255
void WritePNM(BYTE* imgOut, int width, int height, char* filename, int channels)
{
    FILE* fp = fopen(filename, "w");
    if(channels == 3)
        fprintf(fp, "P3\n");
    else if(channels == 1)
        fprintf(fp, "P2\n");
    else
    {
        fprintf(stderr, "Channels argument must be 1 or 3\n");
        return;
    }
    fprintf(fp, "%i %i\n255\n", width, height);


    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            for(int k = 0; k < channels; k++)
            {
                fprintf(fp, "%i ", imgOut[channels*(i*width + j) + k]);
            }
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}