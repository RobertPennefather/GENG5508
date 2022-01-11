//
// Created by GF on 18/10/2017.
//

#include "eyebot.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

void tracking() {
    while(1){
        //DRIVE
        VWStraight(30,200);

        if (PSDGet(1)<100) {
            printf("turn away!\n");
            VWTurn(-20,80);
            if (PSDGet(1)<60){
                VWTurn(-30,80);//turn lots!
            }
        }
        if (PSDGet(1)>200) {
            printf("turn back!\n");
            VWTurn(20,80);
            if (PSDGet(1)>400) {
                VWTurn(30,80);
            }
        }
        if (PSDGet(2)<200) {
            printf("stop!\n");
            break;
        }
    }
}

//navigate around map/drive
int main(){
    int x,y,phi;
    int scan[360];
    VWSetPosition(200,200,0);
    SIMLaserScan(scan);

    //int targetx=3200;
    //int targety=3200;

    bool frontopen, leftopen, rightopen;

    VWStraight(250,500);
    VWWait();
    VWGetPosition(&x,&y,&phi);

    while(1){
        printf("lets go\n");
        frontopen=PSDGet(2)>400;
        leftopen=PSDGet(1)>400;
        rightopen=PSDGet(3)>400;

        /*if(x<400){
            if(y<400){
                printf("im home\n");
                break;
            }
        }*/
        if (leftopen)   {
            printf("left open!\n");
            VWTurn(90,100);
            VWWait();
        }
        else if (frontopen){
            printf("front open!\n");
            tracking();
            VWWait();
            //drive and correct to go along wall
        }
        else if (rightopen) {
            printf("right open!\n");
            VWTurn(-90,100);
            VWWait();
        }
        else {
            VWTurn(180,100);
            VWWait();
        }

        VWStraight(50,100);
        VWWait();//go one step in any case
        VWGetPosition(&x,&y,&phi);
    }
}