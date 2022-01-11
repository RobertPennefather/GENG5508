//gf 21299545
// lab 6
//robotics
//9-10-2017

#include "eyebot.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

#define SCALE 0.2

int nodes[100][2];
int nodenum;
int links[100][100];
int WIDTH;
int HEIGHT;

int distance(int j,int k){ //calc euclidian dist between nodes j and k
    return sqrt(pow(nodes[j][0]-nodes[k][0],2)+pow(nodes[j][1]-nodes[k][1],2));
}

void buildarray(char *filename){
    FILE *file=fopen(filename,'r'); //open file and 'r'-read
    int i=0;
    char *line=malloc(100* sizeof(char)); //malloc=allocates requested memory
    char *token=malloc(100* sizeof(char));
    size_t len; //non-negative variable length

    for (int j = 0; j <100; ++j) { //create links array of -1s
        for (int k = 0; k <100 ; ++k) {
            links[j][k]=-1;
        }
    }


}