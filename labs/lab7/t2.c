//
// Created by GF on 20/10/2017.
//

#include "eyebot.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

int main() {
    int x,y,phi;
    //BYTE *img[120][120];

    VWSetPosition(200,200,0);
    VWStraight(200,200);
    VWWait();
    VWTurn(-90,100);
    VWWait();
    VWStraight(100,200);
    VWWait();
    VWGetPosition(&x,&y,&phi);

    printf("%d,%d,%d\n",x,y,phi);

    COLOR colour=TEAL;
    for (int i = 0; i < 40; ++i) {
        for (int j = 0; j < 90; ++j) {
            LCDPixel(i,j,0xFF0000);
            //img[i][j]=colour;
        }

    }
    //LCDImage(img);

    LCDMenu("ok","","","");
    KEYWait(KEY1);
}