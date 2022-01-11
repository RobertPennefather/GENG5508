//
// Created by GF on 25/10/2017.
//

#include "im.c"
#include "eyebot.h"
#include <stdio.h>
#include "fann.h"

int main(){ //take pics

    CAMInit(QQVGA);
    BYTE buf[QQVGA_SIZE];
    char filename[4];

    for (int j = 0; j < 10; ++j) {
        for (int k = 0; k < 10; ++k) {

            //face object and take pic
            CAMGet(buf);
            sprintf(filename,"SEE/A%d%d.ppm",j,k);
            WritePNM(buf, 160, 120,filename,3);
        }


    }

}