#include <stdio.h>
#include "eyebot.h"
#include "image.c"
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>


BYTE image[128][128];
int free_nodes[10000][3];
int free_count=0;
int occ[10000][3];
int occ_count=0;
//int links[10000][10000];
int paths[10000][4];
int path_count=0;
int dist;

void addFree(int x, int y, int size) {
    free_nodes[free_count][0]=x;
    free_nodes[free_count][1]=y;
    free_nodes[free_count][2]=size;
    free_count++;
}

void addOcc(int x,int y,int size) {
    occ[occ_count][0]=x;
    occ[occ_count][1]=y;
    occ[occ_count][2]=size;
    occ_count++;
}

void quad(int x, int y,int size){ //subdividing
    bool all_free=true, all_occ=true;

    for (int i=x; i<x+size;i++){
        for (int j=y; j<y+size;j++){
            if (image[i][j]) {
                all_free=false;
            }
            else {
                all_occ=false;
            }

        }
    }

    if (all_free) {
        addFree(x,y,size);//store free node
        //printf("(%d,%d)\n",x+size/2,y+size/2);//print centre of nodes
        LCDArea(x,y,x+size,y+size,0x000000,0);
        LCDCircle(x+size/2,y+size/2,4,0xFFFFFF,1);
    }
    else if (!all_occ && size>1) {
        int s2=size/2;
        quad(x,y,s2);
        quad(x+s2,y,s2);
        quad(x,y+s2,s2);
        quad(x+s2,y+s2,s2);
    }
    else {
        addOcc(x,y,size); //store occupied node
        //LCDCircle(x,y,2,0xFFFFFF,1);
    }
}

bool intersection(int path,int k) {
    int F_T, F_U, F_R, F_S;
    int bx = paths[path][2];
    int by = paths[path][3];
    int ax = paths[path][0];
    int ay = paths[path][1];
    int Tx = occ[k][0];
    int Ty = occ[k][1];
    int Ux = occ[k][0] + occ[k][2];
    int Uy = occ[k][1];
    int Rx = occ[k][0];
    int Ry = occ[k][1] + occ[k][2];
    int Sx = occ[k][0] + occ[k][2];
    int Sy = occ[k][1] + occ[k][2];

    //printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",ax,ay,bx,by,Tx,Ty,Ux,Uy,Rx,Ry,Sx,Sy);

    F_T = (by - ay) * Tx + (ax - bx) * Ty + (bx * ay - ax * by);
    F_U = (by - ay) * Ux + (ax - bx) * Uy + (bx * ay - ax * by);
    F_R = (by - ay) * Rx + (ax - bx) * Ry + (bx * ay - ax * by);
    F_S = (by - ay) * Sx + (ax - bx) * Sy + (bx * ay - ax * by);

    if ((F_T > 0 && F_U > 0 && F_R > 0 && F_S > 0) || (F_T < 0 && F_U < 0 && F_R < 0 && F_S < 0)) {
        //collision free
        return false;
    }
        if ((ax > Sx) && (bx > Sx)) return false;
        if ((ax < Tx) && (bx < Tx)) return false;
        if ((ay > Sy) && (by > Sy)) return false;
        if ((ay < Ty) && (by < Ty)) return false;

    return true;//intersection!
}

bool collisionfree(int paths){
    for (int k=0;k<occ_count;k++){
        if (intersection(paths,k))
            return false;
    }
    return true;

}

void createpaths(){
    for(int a=0;a<free_count;a++){
        for (int b=a+1;b<free_count;b++){
            if ((free_nodes[a][2]>=16) && (free_nodes[b][2]>=16)){
                paths[path_count][0]=free_nodes[a][0]+free_nodes[a][2]/2;
                paths[path_count][1]=free_nodes[a][1]+free_nodes[a][2]/2;
                paths[path_count][2]=free_nodes[b][0]+free_nodes[b][2]/2;
                paths[path_count][3]=free_nodes[b][1]+free_nodes[b][2]/2;
                if (collisionfree(path_count)){
                    LCDLine(paths[path_count][0],paths[path_count][1],paths[path_count][2],paths[path_count][3],0x00FF00);//draw line
                    dist=sqrt(pow((paths[path_count][0]-paths[path_count][2]),2)+pow((paths[path_count][1]-paths[path_count][3]),2));//calc dist
                    //printf("%d,%d\n",dist,path_count);
                    path_count++;
                }
            }
        }
    }
}

void drive() {
    int s=4000/128;
    int x1=(free_nodes[0][0]+free_nodes[0][2]/2)*s;
    int y1=4000-(free_nodes[0][1]+free_nodes[0][2]/2)*s;
    int phi=0;
    VWSetPosition(x1,y1,phi);

    //get max x and y for target pos
    int xtarget=paths[0][2]*s;
    int ytarget=4000-paths[0][3]*s;
    //printf("%d\n",xtarget);

    for (int j=1;j<1000;j++) {
        if (xtarget<(paths[j][2]*s) && ytarget>(-paths[j][3])*s) {
            xtarget=paths[j][2]*s;
            ytarget=4000-paths[j][3]*s;
            //printf("%d\n",xtarget);
        }
    }

    printf("%d,%d,%d,%d\n",x1,paths[0][2]*s,y1,4000-paths[0][3]*s);

    //while (x1>xtarget+50 || x1<xtarget-50 || y1>ytarget+50 || y1<ytarget-50)

    for (int i=0;i<10000;i++){
        if (paths[i][0]*s==x1 && (4000-paths[i][1]*s)==y1) {
            int x2=paths[i][2]*s;
            int y2=4000-paths[i][3]*s;

            int dist=sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
            int turn=-90;//+(atan2(y2-y1,x2-x1)*180/M_PI);

            printf("%d,%d\n",dist,turn);

            VWTurn(-90,50);
            VWWait();

            VWStraight(dist,600);
            VWWait();

            VWGetPosition(&x1,&y1,&phi);
            printf("%d,%d,%d,%d\n",x1,xtarget,y1,ytarget);

        }
        if (x1==xtarget && y1==ytarget) {
            printf("%d,%d,%d,%d\n",x1,xtarget,y1,ytarget);
            break;
        }
    }

}


int main() {
    char *filename="blocks.pbm";
    BYTE *img;
    read_pbm(filename,&img);
    LCDArea(0,0,128,128,0xFFFFFF,1);
    LCDImageStart(0,0,128,128);
    LCDImageBinary(img);
    //OSWait(2000);

    for (int i=0;i<128;i++) {
        for (int j=0;j<128;j++) {
            image[j][i]=img[i*128+j];
        }
    }

    quad(0,0,128);
    //printf("%d,%d\n",free_nodes[0][0]+free_nodes[0][2]/2,free_nodes[0][1]+free_nodes[0][2]/2);
    createpaths();
    drive();
    LCDMenu("One", "Two", "Three", "Exit");
    KEYWait(KEY4);

/*
    subdivide(0,0,128);
    displayQuads();
    createPaths();
    displayPaths();
    drive();
    while(1){}
*/
}