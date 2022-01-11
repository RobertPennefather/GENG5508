//gf 21299545
//lab 6
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
int nodenum; //num of nodes
int links[100][100];
int WIDTH;
int HEIGHT;

int distance(int j,int k){ //calc euclidian dist between nodes j and k
    return sqrt(pow(nodes[j][0]-nodes[k][0],2)+pow(nodes[j][1]-nodes[k][1],2));
}

void buildarray(char *filename){ //build links array
    FILE *file=fopen(filename,"r"); //open file and 'r'-read
    int i=0;
    char *line=malloc(100* sizeof(char)); //malloc=allocates requested memory
    char *token=malloc(100* sizeof(char));
    size_t len; //non-negative variable length

    for (int j = 0; j <100; ++j) { //create links array of -1s
        for (int k = 0; k <100 ; ++k) {
            links[j][k]=-1;
        }
    }

    while (getline(&line,&len,file)!=-1){ //change links elements to 1 if connected
        nodes[i][0]=atoi(strtok(line," ")); //could do this a simpler way?
        nodes[i][1]=atoi(strtok(NULL," "));
        token=strtok(NULL," ");
        while (token!=NULL){
            links[i][atoi(token)-1]=1;
            token=strtok(NULL," ");
        }
        i++;
        nodenum++;
    }
    free(line); //deallocates memory
    free(token);

    for (int j = 0; j <nodenum ; ++j) { //input distances if applicable and print
        for (int k = 0; k < nodenum; ++k) {
            if (links[j][k]==1){
                links[j][k]=distance(j,k);
            }
            if (k==nodenum-1){
                printf("%d \n",links[j][k]);
            }
            else {printf("%d ",links[j][k]);}
        }
        fclose(file);
    }
}

void draw_nodes(){
    for (int j = 0; j < nodenum; ++j) {
        LCDCircle(nodes[j][0]*SCALE,nodes[j][1]*SCALE,10,0xFFFFFF,0xFFFFFF);
    }
}

void draw_journey(){
    //LCDLine
}

void draw_paths(){
    for (int j = 0; j < nodenum; ++j) {
        for (int k = 0; k < nodenum; ++k) {
            if (links[j][k]!=-1){
                LCDLine(nodes[j][0]*SCALE,nodes[j][1]*SCALE,nodes[k][0]*SCALE,nodes[k][1]*SCALE,0xFFFFFF);
            }
        }

    }
}

int main() {



    LCDGetSize(&WIDTH,&HEIGHT);
    buildarray("nodes.txt");
    printf("%d\n",nodenum);
    draw_nodes();
    //printf("4/");
    draw_paths();
    //printf("5/");

    LCDMenu("Next","","","");
    KEYWait(KEY1);

    int journey[100]; //store nodes to go to
    int been[100]; //store visited nodes

    int current_node=0; //starting at first node
    int checkednum=1; //number of checked nodes

    int nextnode;
    int j=0;
    journey[j]=current_node;
    been[checkednum-1]=current_node;


    //printf("nodenum %d\n",nodenum);

    while(1){
        //printf("current node %d\n",current_node);
        int min=99999; //set starting min for now

        if (current_node==nodenum-1 ){
            printf("reached goal\n"); //end condition: if at last node, finish
            for (int k = 0; k < j; ++k) {
                printf("%d ",journey[k]);
            }
            return 0;
        }

        for (int k = 0; k < nodenum; ++k) {                         //go through all paths from that node
            if (links[current_node][k]!=-1){                        //if path exists
                for (int past = 0; past < checkednum; ++past) {     //go through visited nodes
                    if (k!=been[past]){                             //if havent been to node
                        if (links[current_node][k]<min){            //check if lower than current min
                            min=links[current_node][k];             //update min
                            nextnode=k;                             //store next node
                        }
                    }
                }
            }
        }



        current_node=nextnode;
        checkednum++;
        j++;
        journey[j]=nextnode;
        been[checkednum-1]=nextnode;

        printf("%d ",checkednum);
    }




    return 0;

    draw_journey();

    printf("Hello, World!\n");
    return 0;
}