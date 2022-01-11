//
// Created by GF on 25/10/2017.
//


#include "im.c"
#include "eyebot.h"
#include <stdio.h>
#include "fann.h"

void trackdist() { //one iteration

    for (int i = 0; i < 10; ++i) {
        VWStraight(20,400);
        if (PSDGet(3)<100){
            VWTurn(10,60);//turn away
            if (PSDGet(3)<80){
                VWTurn(14,60);//turn lots!
            }
        }
        if (PSDGet(3)>200) {
            VWTurn(-6,200);//turn back
            if (PSDGet(3)>400) {
                VWTurn(-10,40);//turn back lots!
            }
        }
    }
}

void circle() {
    int dist=round((2*M_PI*340)/100);
    int angle=-3;
    VWCurve(dist,angle,100);
}


int main(){

    CAMInit(QQVGA);
    BYTE buf[QQVGA_SIZE];
    char filename[4];

    while(1){ //to go object and turn left
        VWStraight(60,400);
        if (PSDGet(2)<240){
            VWSetSpeed(0,0);
            VWTurn(90,100);//turn left
            VWWait();
            break;
        }
    }

    for (int j = 0; j < 10; ++j) {
        for (int k = 0; k < 10; ++k) {

            circle();
            OSWait(1000);

            VWTurn(-90,400); //face obj
            VWWait();

            OSWait(1000);

            VWTurn(90,400); //face back
            VWWait();

            //take pic
            CAMGet(buf);
            sprintf(filename,"SEE/A%d%d.ppm",j,k);
            WritePNM(buf, 160, 120,filename,3);
        }
    }
}