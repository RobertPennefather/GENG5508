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

BYTE image[QQVGA_SIZE];

void collectImages(){
	int count=0;
	int right;
	int threshold;
	while (count<100){
		//Set position to 0,0,0
		VWSetPosition(0,0,0);
		//Turn 90º
		VWTurn(90,50);
		VWWait();
		//Get and store PSD Right
		right=PSDGet(3);
		printf("right=%d\n",right);
		threshold=10;
		//initialise position vars to outside bounds
		int x=21;
		int y=21;
		int phi=0;
		//while not at start
		while ((abs(x)>20 || abs(y)>20)&&(count<100)){
			for (int i=0;i<4;i++){
				//Drive a bit
				if ((PSDGet(3)>(right-threshold)) && (PSDGet(3)<(right+threshold))){	//at a good distance from object
					VWCurve(10,-2,100);
					VWWait();
				}
				else if (PSDGet(3)<(right-threshold)){	//too close to object
					VWCurve(10,3,100);
					VWWait();
				}
				else if (PSDGet(3)>(right+threshold)){	//too far away from object
					VWCurve(10,-3,100);
					VWWait();
				}
			}
			//Turn towards object
			VWTurn(-90,100);
			VWWait();
			//Take pic and save
			CAMGet(image);
			
			//save image
			char filename[3];
			sprintf(filename,"C/C%d.ppm",count);
			WritePNM(image,IMAGE_WIDTH,IMAGE_HEIGHT,filename,3);
			
			//Turn back to path
			VWTurn(90,100);
			VWWait();
			
			//increment counter for pictures acquired
			count++;
			
			//Check position
			VWGetPosition(&x,&y,&phi);
			printf("count=%d\n",count);
		}
		KEYWait(KEY1);
		VWTurn(-90,100);
		VWWait();
		VWStraight(-100,100);
		VWWait();
	}
	
	
}

int main(){
	
	//Place robot facing object
	//Initialise camera
	CAMInit(QQVGA);
	
	//-------------------------------------------
	
	//Initialise LCD Screen
	LCDClear();
	LCDMenu("One", "Two", "Three", "Four");
	printf("\nPress Key 1 to draw nodes.\n");
	KEYWait(KEY1);
	
	//-------------------------------------------
	
	//
	collectImages();
	
	
	//-------------------------------------------
	
	
	//-------------------------------------------
	
	printf("\nPress Key 2 to draw node connections.\n");
	KEYWait(KEY2);
	
	//-------------------------------------------
	
	printf("\nPress Key 3 to perform the A* algorithm.\n");
	KEYWait(KEY3);
	
	//-------------------------------------------

	//Driving robot
	printf("\nPress Key 4 for driving.\n");
	KEYWait(KEY4);
	
	printf("\nDriving complete.\n\nPress any key to exit.");
	
	//-------------------------------------------
	
	//printf("\nPress any key to exit.");
	
	LCDMenu("Exit","Exit","Exit","Exit");
	KEYWait(ANYKEY);
}
