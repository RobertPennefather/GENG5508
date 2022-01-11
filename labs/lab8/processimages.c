//Gurashish Singh Bhatia, 21324773
//Junho Jung, 21302695

#include <math.h>
#include "eyebot.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "im.h"
//#include "fann.h"

#define SCALE 1

#define IMAGE_WIDTH 160
#define IMAGE_HEIGHT 120

BYTE in_image[QQVGA_SIZE];
BYTE out_image[144];

void downImages(){
	char in_filename[10];
	//char out_filename[14];
	
	char *filename;
	filename="data.data";
	FILE *data_file=fopen(filename, "w");
	fprintf(data_file,"300 144 3\n");
	//for each file
	for (int num=0;num<100;num++){
		//printf("num1=%d",num);
		//read file
		sprintf(in_filename,"A/A%d.ppm",num);
		//printf("num2=%d",filenum);
		ReadPNM(in_filename,in_image);
		//printf("num3=%d",num);
		
		//downsize image
		DownscaleImage(in_image,IMAGE_WIDTH,IMAGE_HEIGHT,out_image,8,6);
		
		//printf("%d",out_image[0]);
		
		//print data
		for (int i=0;i<144;i++){
			//for (int j=0;j<6;j++){
			fprintf(data_file,"%d ",out_image[i]);
			//}
		}
		fprintf(data_file,"\n1 -1 -1\n");
		//write to file
		//sprintf(out_filename,"C/C%d_down.ppm",num);
		//printf("num4=%d",num);
		//printf("outfile=%s\n",out_filename);
		//WritePNM(out_image,8,6,out_filename,3);
	}
	
	for (int num=0;num<100;num++){
		//printf("num1=%d",num);
		//read file
		sprintf(in_filename,"B/B%d.ppm",num);
		//printf("num2=%d",filenum);
		ReadPNM(in_filename,in_image);
		//printf("num3=%d",num);
		
		//downsize image
		DownscaleImage(in_image,IMAGE_WIDTH,IMAGE_HEIGHT,out_image,8,6);
		
		//printf("%d",out_image[0]);
		
		//print data
		for (int i=0;i<144;i++){
			//for (int j=0;j<6;j++){
			fprintf(data_file,"%d ",out_image[i]);
			//}
		}
		fprintf(data_file,"\n-1 1 -1\n");
		//write to file
		//sprintf(out_filename,"C/C%d_down.ppm",num);
		//printf("num4=%d",num);
		//printf("outfile=%s\n",out_filename);
		//WritePNM(out_image,8,6,out_filename,3);
	}
	
	for (int num=0;num<100;num++){
		//printf("num1=%d",num);
		//read file
		sprintf(in_filename,"C/C%d.ppm",num);
		//printf("num2=%d",filenum);
		ReadPNM(in_filename,in_image);
		//printf("num3=%d",num);
		
		//downsize image
		DownscaleImage(in_image,IMAGE_WIDTH,IMAGE_HEIGHT,out_image,8,6);
		
		//printf("%d",out_image[0]);
		
		//print data
		for (int i=0;i<144;i++){
			//for (int j=0;j<6;j++){
			fprintf(data_file,"%d ",out_image[i]);
			//}
		}
		fprintf(data_file,"\n-1 -1 1\n");
		//write to file
		//sprintf(out_filename,"C/C%d_down.ppm",num);
		//printf("num4=%d",num);
		//printf("outfile=%s\n",out_filename);
		//WritePNM(out_image,8,6,out_filename,3);
	}
	
	fclose(data_file);
	
}

int main(){
	
	//Place robot facing object
	//Initialise camera
	//CAMInit(QQVGA);
	
	//-------------------------------------------
	
	//Initialise LCD Screen
	LCDClear();
	LCDMenu("One", "Two", "Three", "Four");
	printf("\nPress Key 1 to draw nodes.\n");
	KEYWait(KEY1);
	
	//-------------------------------------------
	
	//
	downImages();
	
	//-------------------------------------------
	
	//printf("\nPress any key to exit.");
	
	LCDMenu("Exit","Exit","Exit","Exit");
	KEYWait(ANYKEY);
}
