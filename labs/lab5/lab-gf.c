
/* GENG5508 Robotics
 * Semester 2, 2017
 * Lab 5
 * Aaron Hurst, 21325887
 * Gwendolyn Foo, 21299545
 *
 * ACKNOWLEDGEMENTS:
 * Connected component analysis code (fill_region and find_obstacles) inspired
 * by code described here:
 * https://stackoverflow.com/questions/14465297/connected-component-labelling
 */

#include <stdio.h>		    // printf
#include <math.h>		    // pow, sqrt
//#include <winnt.h>
#include "eyebot.h"		    // EyeBot 7 library
#include "image.c"		    // read pbm image

#define IMG_SIZE	128		// size of input image/map

#define SUCCESS     1
#define FAIL        0

bool map[IMG_SIZE][IMG_SIZE];       // binary image map
COLOR regions[IMG_SIZE][IMG_SIZE];  // array of region identifiers (unique colours)
BYTE brush_out[IMG_SIZE*IMG_SIZE];  // image showing output of brushfire algorithm
BYTE brush_map[IMG_SIZE][IMG_SIZE]; // brushfire over map array
int voronoi[10000][2];              // voronoi points

COLOR colours_list[15] = {RED, GREEN, BLUE, ORANGE, SILVER, LIGHTGRAY, DARKGRAY, NAVY, CYAN, TEAL, MAGENTA, PURPLE, MAROON, YELLOW, OLIVE};


//======= fill_region
/* This function receives a given point in the image as its input and checks
 * whether it is part of the current region. If so, it is labelled accordingly
 * with the current colour. The function is then called recursively on all of
 * its eight neighbours, thereby filling in the entire region.
 */
void fill_region(int x, int y, int region_cur)
{
    // Check if point (x,y) is not part of the current region
    if (!map[x][y])             return;     // pixel not occupied
    if (regions[x][y] > 0)      return;     // already labelled.. this check should not be necessary
    if (x < 0 || x >= IMG_SIZE) return;     // out of bounds
    if (y < 0 || y >= IMG_SIZE) return;     // out of bounds

    // Point is part of current region, label accordingly
    regions[x][y] = colours_list[region_cur];
    brush_out[x*IMG_SIZE + y] = colours_list[region_cur];

    // Recursively call fill_region on all 8 neighbours
    fill_region(x-1, y-1, region_cur);
    fill_region(x-1, y  , region_cur);
    fill_region(x-1, y+1, region_cur);
    fill_region(x  , y-1, region_cur);
    fill_region(x  , y+1, region_cur);
    fill_region(x+1, y-1, region_cur);
    fill_region(x+1, y  , region_cur);
    fill_region(x+1, y+1, region_cur);

    return;
}

//======= find_obstacles
/* This function finds each obstacle (connected region) and fills out the
 * corresponding entries in the regions array with a matching and unique
 * colour to identify the obstacle.
 *
 * return: the number of regions found
 */
int find_obstacles(void)
{
    int n_regions = 0;
    for (int i = 0; i < IMG_SIZE; i++) {
        for (int j = 0; j < IMG_SIZE; j++) {
            if (map[i][j] && regions[i][j] == 0) {
                // Occupied but not yet marked
                fill_region(i, j, n_regions);
                n_regions++;
            }
        }
    }
    return n_regions;
}




int nowhitespace() { //check if done brushfiring
    for (int i=0;i<IMG_SIZE;i++) {
        for (int j=0;j<IMG_SIZE;j++){
            if (brush_map[i][j]==0)       return 0; // false
            //else if  (i==IMG_SIZE-1 && j==IMG_SIZE-1)        return 1; // true
            // else keep iterating
        }
    }
    return 1;
}

// handle first iteration outside the main loop
// you're never going to hit/go outside the boundaries of your array after the first iteration
void firstbrushfire(){
    int iteration=2;
    //do first iteration
    for (int i=0;i<IMG_SIZE;i++){
        brush_map[i][0]=iteration; // left column
        regions[i][0]=colours_list[14]; //set colour to olive
        LCDPixel(IMG_SIZE + 10 , i, regions[i][0]);
    }
    for (int i=0;i<IMG_SIZE;i++) {
        brush_map[i][IMG_SIZE-1]=iteration; //right column
        regions[i][IMG_SIZE-1]=colours_list[13]; //set colour to yellow
        LCDPixel(IMG_SIZE + 10 + IMG_SIZE-1, i, regions[i][IMG_SIZE-1]);
    }
    for (int i=0;i<IMG_SIZE;i++) {
        brush_map[0][i]=iteration; //top row
        regions[0][i]=colours_list[12]; //set colour to maroon
        LCDPixel(IMG_SIZE + 10+i, 0, regions[0][i]);
    }
    for (int i=0;i<IMG_SIZE;i++) {
        brush_map[IMG_SIZE-1][i]=iteration; //bottom row
        regions[IMG_SIZE-1][i]=colours_list[12]; //set colour to purple
        LCDPixel(IMG_SIZE + 10+i, IMG_SIZE-1, regions[IMG_SIZE-1][i]);
    }

    // set 4 voronoi points in corners
    voronoi[0][0]=0; // top left
    voronoi[0][1]=0;

    voronoi[1][0]=IMG_SIZE-1; // top right
    voronoi[1][1]=0;

    voronoi[2][0]=0; // bottom left
    voronoi[2][1]=IMG_SIZE-1;

    voronoi[3][0]=IMG_SIZE-1; // bottom right
    voronoi[3][1]=IMG_SIZE-1;
}

void brushfire() {
    int iteration=3;

    while(1){
        int n_voronoi=4; // number of voronoi points

        for (int i=0;i<IMG_SIZE;i++){
            for (int j=0;j<IMG_SIZE;j++){

                if (brush_map[i][j]==iteration-1) {// find last iteration

                    for (int a=-1;a<2;a++){
                        for (int b=-1;b<2;b++){ // look at all neighbours

                            if (a==0 && b==0){
                                // existing obstacle point... skip

                                //printf("skip\n");
                            }
                            else if (regions[i+a][j+b]>0){
                                //existing obstacle .. skip
                            }
                            else if(brush_map[i+a][j+b]==iteration){ //overlap same value, diff colour
                                if (regions[i+a][j+b]==regions[i][j]){ // same colour
                                    //not voronoi.. do nothing
                                }
                                //diff colour
                                voronoi[n_voronoi][0]=i; // set as voronoi point
                                voronoi[n_voronoi][1]=j;
                                n_voronoi++;

                                LCDPixel(IMG_SIZE + 10+i+a, j+b, 0xFFFFFF);
                                //printf("found voronoi\n");
                            }
                            else if (brush_map[i+a-1][j+b]==iteration) { // if same iteration on top
                                if (regions[i+a-1][j+b]==regions[i][j]) { // and same colour
                                    //not voronoi .. check right side
                                    if (brush_map[i+a][j+b+1]==iteration) { // if same value on right
                                        if (regions[i+a][j+b+1]==regions[i][j]) { // and same colour
                                            //not voronoi
                                            brush_map[i+a][j+b]=iteration;
                                            regions[i+a][j+b]=regions[i][j]; // set colour of neighbour to colour of obstacle
                                            LCDPixel(IMG_SIZE + 10 + i, j, regions[i+a][j+b]);
                                        }
                                        //diff colour.. then voronoi
                                        voronoi[n_voronoi][0]=i+a; // set as voronoi point
                                        voronoi[n_voronoi][1]=j+b;
                                        n_voronoi++;
                                        LCDPixel(IMG_SIZE + 10+i+a, j+b, 0x0000FF);
                                    }

                                }
                                //voronoi
                                voronoi[n_voronoi][0]=i+a; // set as voronoi point
                                voronoi[n_voronoi][1]=j+b;
                                n_voronoi++;
                                LCDPixel(IMG_SIZE + 10+i+a, j+b, 0xFFFFFF);

                            }
                            else { // mark new point
                                brush_map[i+a][j+b]=iteration;
                                regions[i+a][j+b]=regions[i][j]; // set colour of neighbour to colour of obstacle


                                //LCDPixel(IMG_SIZE + 10 + i, j, regions[i+a][j+b]);
                            }
                        }
                    }
                    iteration++;
                }
            }
        }

        if (nowhitespace())        break; //done

    }


    //loop over all elments in grid

    // put iteration number in all of its neighbours
    // BUT: if you already see a point with the same value of a different colour, mark as voroni

    // if not a voroni point, set value to current iteration and colour to whichever obstacle you are exanding from

    // Don't forget the second condition for Voronoi points - can be a bit hard to implement
    // The tricky bit is the order in which you loop through the array
}

//continue until you don't see any whitespace

/*
// DRIVE
void Drive()
{
    // Encode starting position
    // By default it thinks it's in the bottom right corner
    VWSetPositoin(250,250,0);       // everything is calculated from here

    // Select a path
    // Doesn't matter which path - not looking for most efficient
    // Watch out for points on the path being too close to you - choose one about 100-150mm in front of the robot
    xPoints[100];
    yPoints[100];

    // Fil x and y arrays
    for each point
        drive to ith waypoint




}
// Image coordinates: positive y is down, positive x is to the right
// Robot coordinates: positive is forward, y is the right
// need to convert between these when doing drive
*/



int main (/*int argc, char **argv*/)
{
    /*
    // Read binary image
    BYTE *img;
    if (argc < 2)
    {
        printf("\nERROR: You must specify which image to use as a command line argument.\n");
        printf("Available options are:\n");
        printf("1: rectangles\n");
        printf("2: fourblocks\n");
        printf("3: u\n");
        return FAIL;
    }
    else if (atoi(argv[1]) == 1)		read_pbm("rectangles.pbm", &img);
    else if (atoi(argv[1]) == 2)		read_pbm("fourblocks.pbm", &img);
    else if (atoi(argv[1]) == 3)		read_pbm("u.pbm", &img);
    else {
        printf("ERROR: Invalid argument (only accepts 1, 2 or 3)\n");
        return FAIL;
    }
     */

    char *filename="fourblocks.pbm";
    BYTE *img;
    read_pbm(filename,&img);

    // Display image
    LCDArea(0, 0, IMG_SIZE, IMG_SIZE, 0xFFFFFF, 1);
    LCDImageStart(0, 0, IMG_SIZE, IMG_SIZE);
    LCDImageBinary(img);

    LCDMenu("Next","","","");
    KEYWait(KEY1);

    // Convert to a two dimensional array
    for (int i = 0; i < IMG_SIZE; i++) {
        for (int j = 0; j < IMG_SIZE; j++) {
            map[j][i] = (img[i*IMG_SIZE + j] == 1);
        }
    }

    // Identify obstacles, store output in regions array
    int n_obstacles;      // number of distinct regions (obstacles)
    n_obstacles = find_obstacles();
    printf("\nNumber of obstacles: %d\n", n_obstacles);

    // Show obstacles in colour on LCD
    for (int i = 0; i < IMG_SIZE; i++) {
        for (int j = 0; j < IMG_SIZE; j++) {
            if (regions[i][j] > 0) {
                LCDPixel(IMG_SIZE + 10 + i, j, regions[i][j]);
            }
        }
    }

    LCDMenu("Next","","","");
    KEYWait(KEY1);


    // Brushfire
    // copy over map array
    for (int i = 0; i < IMG_SIZE; i++) {
        for (int j = 0; j < IMG_SIZE; j++) {
            brush_map[i][j]=map[i][j];
        }
    }

    printf("do the brushfire\n");

    firstbrushfire();

    LCDMenu("Next","","","");
    KEYWait(KEY1);

    printf("do the rest of brushfire\n");
    brushfire();

    printf("first voronoi point %d,%d\n", voronoi[0][0],voronoi[0][0]);

    for (int i = 0; i < IMG_SIZE; i++) {
        for (int j = 0; j < IMG_SIZE; j++) {
            if (regions[i][j] > 0) {
                LCDPixel(2*IMG_SIZE + 20 + i, j, regions[i][j]);
            }
        }
    }


    // Drive




    return 0;

}