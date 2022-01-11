#include "eyebot.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

int x,y,phi;

int scan[360];
//BYTE *image[120][120];
//COLOR colour=TEAL;
int walldist;
int xx,yy;

void map(){
    for (int i = 0; i < 360; ++i) {
        VWGetPosition(&x,&y,&phi);
        SIMLaserScan(scan);
        walldist=scan[i];
        xx=walldist*(sin((i+phi)*M_PI/180));
        yy=walldist*(cos((i+phi)*M_PI/180));
        //printf("%d,%f,%f,%d\n",i,xx,yy,walldist);
        //image[(x+xx)/30][-((y+yy)/30)]=colour;

        LCDPixel(120-((y+xx)/30),130-((x+yy)/30),0xFF0000);
    }
}

void tracking() {
    while(1){
        //DRIVE
        VWStraight(50,200);



        if (PSDGet(1)<100) {
            VWTurn(-20,60);//turn away
            if (PSDGet(1)<60){
                VWTurn(-30,60);//turn lots!
            }
        }
        if (PSDGet(1)>200) {
            VWTurn(20,60);//turn back
            if (PSDGet(1)>400) {
                VWTurn(30,60);//turn back lots!
            }
        }
        if (PSDGet(2)<200) {
            break;
        }
    }
}

//navigate around map/drive
void nav(targetx,targety){
    bool frontopen, leftopen, rightopen;

    while (1){
        frontopen=PSDGet(2)>300;
        leftopen=PSDGet(1)>300;
        rightopen=PSDGet(3)>300;


        VWGetPosition(&x,&y,&phi);
        if (x<400 && abs(y)<400){
            printf("%d,%d\n",x,y);
            break;

        }
        else if (leftopen)   {
            VWTurn(90,100);
            VWWait();
        }
        else if (frontopen){
            map();
            tracking();
            VWWait();
            //drive and correct to go along wall
        }
        else if (rightopen)  {
            VWTurn(-90,100);
            VWWait();
        }
        else {
            VWTurn(180,100);
            VWWait();
        }
        VWStraight(50,100);
        VWWait();
        //go one step in any case
    }
}


int main() {

    VWSetPosition(200,200,0);
    VWStraight(250,200);
    VWWait();
    nav();

    //LCDImage(image);

    LCDMenu("ok","","","");
    KEYWait(KEY1);

    printf("Hello, World!\n");
    return 0;
}