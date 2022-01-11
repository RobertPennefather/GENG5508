//
// Created by GF on 14/09/2017.
//
#include <stdio.h>
#include <stdlib.h>
#include "eyebot.h"
#include <math.h>

int scan[360];
int x,y,phi;
int targetx, targety; //relative to initial pos

void Drivetotarget() {

    //control drive
    while (1) {
        //get current position and target relative to current
        VWGetPosition(&x, &y, &phi);
        int dx = targetx - x;
        int dy = targety -y; // positive to right of robot

        //get angle to turn (angle to be at minus current angle)
        float angle = atan2(dy, dx); //in rad
        float angletoturn = angle * 180 / M_PI - (float) phi; //in deg

        int front = PSDGet(2);
        int left = PSDGet(1);
        int right = PSDGet(3);
        int threshold = 200;

        VWTurn(round(angletoturn),60);
        VWStraight(sqrt(dx*dx+dy*dy), 200);
        if (angletoturn < -5) {
            VWTurn(round(angletoturn), 60);//turn right
        }
        if (angletoturn > 5) {
            VWTurn(round(angletoturn), 60);//turn left
        }
        if (left < threshold) {
            VWTurn(-10, 60);//curve away from left obstacle
        }
        if (right < threshold) {
            VWTurn(10, 60);//curve away from right obstacle
        }
        if ((targetx - 15 < x && x < targetx + 15) && (targety - 15 < y && y < targety + 15)) {
            printf("WINNER\n");
            printf("x=%d,y=%d,phi=%d\n",x,y,phi);
            exit(0);
        }
        if (front < threshold) {
            VWSetSpeed(0, 0);
            printf("hold up wat dat\n");
            break;
        }
    }
}

void Alignwithwall() { //turn left parallel to obstacle

    SIMLaserScan(scan); //scan distances all around

    int obstacleangle=0;
    int mindist=scan[0];//find angle with min dist (this will be the obstacle)
    for (int i=1;i<359;i++) {
        if (mindist>scan[i]) {
            mindist=scan[i];
            obstacleangle=i;
        }
    }

    VWTurn(obstacleangle-270,60); //align with wall!

    VWWait();
}

void TrackObstacle() {

    VWGetPosition(&x,&y,&phi);
    int hitx=x;
    int hity=y;

    int STEP=800;

    VWStraight(60,100); //get away from hitpoint so not to end loop
    VWWait();


    while(1) {

        VWGetPosition(&x,&y,&phi);
        int dx=targetx-x;
        int dy=targety-y;
        int dcurrent=sqrt(dx*dx+dy*dy);
        int dmin=dcurrent;

        float angle=atan2(dy,dx);
        int angletoturn=round(angle*180/M_PI-(float)phi); //???????????????????????????????????????????????????
        //CALC FREESPACE
        SIMLaserScan(scan);
        if (angletoturn<0) {
            angletoturn=360+angletoturn;
        }
        float F=scan[angletoturn];

        //DRIVE
        VWStraight(1000,100);

        //TRACK
        if (dcurrent<dmin){
            dmin=dcurrent;
        }
        if (PSDGet(3)<100) {
            VWTurn(20,60);//turn away
            if (PSDGet(3)<60){
                VWTurn(30,60);//turn lots!
            }
        }
        if (PSDGet(3)>200) {
            VWTurn(-20,60);//turn back
            if (PSDGet(3)>400) {
                VWTurn(-30,60);//turn back lots!
            }
        }
        if (dcurrent-F<=dmin-STEP) {
            VWSetSpeed(0,0);
            printf("free\n");
            printf("x=%d,y=%d\n",x,y);
            VWStraight(60,100); //clear the obstacle
            VWWait();
            VWTurn(-phi,60);
            VWWait();
            break;
        }
        if ((x>hitx-30 && x<hitx+30) && (y>hity-30 && y<hity+30)) {
            VWSetSpeed(0, 0);
            printf("nope\n");
            printf("hitx=%d,hity=%d\n",hitx,hity);
            exit(0);
        }
    }
}

int main() {
    VWSetPosition(0,0,0);
    targetx=1000; //positive in front of robot
    targety=-1000; //positive to left of robot
    VWGetPosition(&x, &y, &phi);

    while (abs(targetx-x)>14 && abs(targety-y)>14){ //(targetx - 15 > x && x > targetx + 15) && (targety - 15 > y && y > targety + 15)) {
        Drivetotarget();
        Alignwithwall();
        TrackObstacle();

    }
    printf("WINNER\n");
    printf("x=%d,y=%d,phi=%d\n",x,y,phi);
}

/*
int main() {
    int x,y,phi;
    VWSetPosition(0,0,0);
    VWDrive(100,200,40);
    VWWait();
    VWGetPosition(&x, &y, &phi);
    printf("x=%d,y=%d\n",x,y);
}
 */