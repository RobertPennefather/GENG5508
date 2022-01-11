//
// Created by GF on 6/10/2017.
//

/* GENG5508 Robotics
 * Semester 2, 2017
 * Lab 5
 * Aaron Hurst, 21325887
 *
 * ACKNOWLEDGEMENTS:
 * Connected component analysis code (fill_region and find_obstacles) inspired
 * by code described here:
 * https://stackoverflow.com/questions/14465297/connected-component-labelling
 */

#include <stdio.h>		    // printf
#include <math.h>		    // pow, sqrt
#include "eyebot.h"		    // EyeBot 7 library
#include "image.c"		    // read pbm image

#define IMG_SIZE	128		// size of input image/map

#define N_WALL_COL  RED
#define E_WALL_COL  GREEN
#define S_WALL_COL  BLUE
#define W_WALL_COL  ORANGE

#define SUCCESS     1
#define FAIL        0

// GLOBALS
bool map[IMG_SIZE][IMG_SIZE];           // binary image map
bool voronoi[IMG_SIZE][IMG_SIZE];       // map of voronoi points
int labels[IMG_SIZE][IMG_SIZE];         // iteration in which point was labelled (previous iterations)
int labels_new[IMG_SIZE][IMG_SIZE];     // current iteration only
COLOR colours[IMG_SIZE][IMG_SIZE];      // region identifying colours (previous iterations)
COLOR colours_new[IMG_SIZE][IMG_SIZE];  // current iteration only

COLOR colours_list [11] = {YELLOW, SILVER, NAVY, CYAN, TEAL, MAGENTA, PURPLE, MAROON, OLIVE, LIGHTGRAY, DARKGRAY};

int dirx [8] = {-1,-1,-1, 0, 0, 1, 1, 1};
int diry [8] = {-1, 0, 1,-1, 1,-1, 0, 1};


int get_col(COLOR col)
{
    for (int i = 0; i < 11; i++) {
        if (col == colours_list[i]) {
            return i+4;
        }
    }
    if (col == N_WALL_COL) return 1;
    if (col == E_WALL_COL) return 2;
    if (col == S_WALL_COL) return 3;
    if (col == W_WALL_COL) return 4;

    return -1;
}


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
    if (colours[x][y] > 0)      return;     // already labelled.. this check should not be necessary
    if (x < 0 || x >= IMG_SIZE) return;     // out of bounds
    if (y < 0 || y >= IMG_SIZE) return;     // out of bounds

    // Point is part of current region, label accordingly
    colours[x][y] = colours_list[region_cur];
    //brush_out[x*IMG_SIZE + y] = colours_list[region_cur];

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

//======= brushfire_map_walls
/* First pass of the Brushfire algorithm
 * returns: void
 */
void brushfire_map_walls(int iter)
{
    // Label map walls (note that this checks that each wall point is not part of an obstacle)
    for (int i = 0; i < IMG_SIZE; i++) {
        if (!map[i][0]) // north wall
        {
            colours[i][0] = N_WALL_COL;
            labels[i][0] = iter;
        }
        if (!map[IMG_SIZE-1][i]) // east wall
        {
            colours[IMG_SIZE-1][i] = E_WALL_COL;
            labels[IMG_SIZE-1][i] = iter;
        }
        if (!map[i][IMG_SIZE-1])  // south wall
        {
            colours[i][IMG_SIZE-1] = S_WALL_COL;
            labels[i][IMG_SIZE-1] = iter;
        }
        if (!map[0][i]) // west wall
        {
            colours[0][i] = W_WALL_COL;
            labels[0][i] = iter;
        }
    }

    // Label corners
    voronoi[0][0] = true;
    voronoi[0][IMG_SIZE-1] = true;
    voronoi[IMG_SIZE-1][0] = true;
    voronoi[IMG_SIZE-1][IMG_SIZE-1] = true;
    colours[0][0] = 0;
    colours[0][IMG_SIZE-1] = 0;
    colours[IMG_SIZE-1][0] = 0;
    colours[IMG_SIZE-1][IMG_SIZE-1] = 0;

    return;
}

//======= find_obstacles
/* This function finds each obstacle (connected region) and fills out the
 * corresponding entries in the colours array with a matching and unique
 * colour to identify the obstacle.
 *
 * return: the number of regions found
 */
int find_obstacles(void)
{
    int n_regions = 0;
    for (int i = 0; i < IMG_SIZE; i++) {
        for (int j = 0; j < IMG_SIZE; j++) {
            if (map[i][j] && colours[i][j] == 0) {
                // Occupied but not yet marked
                fill_region(i, j, n_regions);
                n_regions++;
            }
        }
    }
    return n_regions;
}

//======= check_nbrs_labels
/* return: iteration number (if labelled neighbour found) of 0
 */
int check_nbrs_labels(int x, int y, int iter)
{
    // Check each eight neighbours if they were labelled in previous iteration
    for (int i = 0; i < 8; i++) {
        if (labels[x+dirx[i]][y+diry[i]] == iter-1) {
            return iter;
        }
    }
    return 0;
}

//======= check_nbrs_colours
/* Check colours of each eight neighbours and return appropriate colour
 * to label current pixel
 * Also checks if point is a voronoi point
 *
 * returns: colour to apply, 0 (if voronoi point)
 */
COLOR check_nbrs_colours(int x, int y)
{
    int nbrs = 0;
    COLOR nbrs_colours [8];

    // Read colours of all neighbours which are labelled (specifically excluding voronoi points)
    for (int i = 0; i < 8; i++) {
        if (colours[x+dirx[i]][y+diry[i]] > 0 && !voronoi[x+dirx[i]][y+diry[i]]/*second check should be superfluous*/) {
            nbrs_colours[nbrs] = colours[x+dirx[i]][y+diry[i]];
            nbrs++;
        }
    }

    // Check which colour to return
    if (nbrs > 1) {
        // Check if more than one colour was encountered
        for (int i = 0; i < nbrs; i++) {
            for (int j = i+1; j < nbrs; j++) {
                if (nbrs_colours[i] != nbrs_colours[j]) {
                    voronoi[x][y] = true;
                    //printf("Standard Voronoi (%d,%d)\n", x, y);
                    return 0;
                }
            }
        }
        return nbrs_colours[0];
    }
    else if (nbrs == 1) {
        return nbrs_colours[0];
    }

    printf("ERROR: no neighbours with any colour!! (%d,%d)\n", x, y);

    return 0;
}

//======= brushfire_iter
/*
 */
bool brushfire_iter(int iter)
{
    bool change = false;

    /* STEP 1: Update labels_new
     * Loop over map, if pixel unlabelled, check its neighbours
     * Label pixel with iter if any neighbours in labelled in previous iteration
     */
    for (int i = 0; i < IMG_SIZE; i++) {
        for (int j = 0; j < IMG_SIZE; j++) {
            if (labels[i][j] == 0) {
                labels_new[i][j] = check_nbrs_labels(i,j,iter);
            }
        }
    }

    /* STEP 2: Update colours, check for Voronoi with multi-colour neighbours
     */
    for (int i = 0; i < IMG_SIZE; i++) {
        for (int j = 0; j < IMG_SIZE; j++) {
            if (labels_new[i][j] == iter) {
                colours_new[i][j] = check_nbrs_colours(i,j);
            }
        }
    }

    /* STEP 3: Check for Voronoi points with differently coloured top/right neighbours
     * Check for Voronoi points based on their top and/or right neighbours in the current
     * iteration. Loops through all pixels labelled in the current iteration. I either
     * top or right neighbour is labelled in the current interation and has a different
     * colour, this is a voronoi point.
     */
    for (int i = 0; i < IMG_SIZE; i++) {
        for (int j = 0; j < IMG_SIZE; j++) {
            if (labels_new[i][j] == iter && !voronoi[i][j]) {
                // Check top neighbour
                if (labels_new[i][j-1] == iter && !voronoi[i][j-1] && (colours_new[i][j-1] != colours_new[i][j])) {
                    //printf("Special Voronoi TOP   (%3d,%3d)", i, j);
                    //printf("  colours_new self = %i; colours_new TOP = %d\n", get_col(colours_new[i][j]), get_col(colours_new[i][j-1]));
                    voronoi[i][j] = true;
                    colours_new[i][j] = 0;
                }
                // Check right neighbour
                if (labels_new[i+1][j] == iter && !voronoi[i+1][j] && (colours_new[i+1][j] != colours_new[i][j])) {
                    //printf("Special Voronoi RIGHT (%3d,%3d)", i, j);
                    //printf("  colours_new self = %i; colours_new TOP = %d\n", get_col(colours_new[i][j]), get_col(colours_new[i+1][j]));
                    voronoi[i][j] = true;
                    colours_new[i][j] = 0;
                }
            }
        }
    }

    /* STEP 4: Copy labels_new and colours_new back to main arrays
     */
    for (int i = 0; i < IMG_SIZE; i++) {
        for (int j = 0; j < IMG_SIZE; j++) {
            if (labels_new[i][j] == iter) {
                // Copy
                labels[i][j] = labels_new[i][j];
                colours[i][j] = colours_new[i][j];
                // Reset
                labels_new[i][j] = 0;
                colours_new[i][j] = 0;
                // Record that at least one change has occurred
                if (!change)    change = true;
            }
        }
    }

    // Print Brushfire output
    // for (int i = 0; i < IMG_SIZE; i++) {
    //     for (int j = 0; j < IMG_SIZE; j++) {
    //         if (colours[i][j] > 0) {
    //             LCDPixel(2*(IMG_SIZE + 10) + i, j, colours[i][j]);
    //         }
    //     }
    // }

    // Print Voronoi points
    // for (int i = 0; i < IMG_SIZE; i++) {
    //     for (int j = 0; j < IMG_SIZE; j++) {
    //         if (voronoi[i][j]) {
    //             LCDPixel(i, IMG_SIZE + 10 + j, RED);
    //             LCDPixel(j, i, RED);
    //         }
    //     }
    // }

    //KEYWait(KEY1);

    return change;
}

//======= brushfire
/*
 */
int brushfire(void)
{
    // Variables
    int iteration = 2;
    bool at_least_one_change = true;

    // Label points adjacent to map walls
    brushfire_map_walls(iteration);

    // Main loop
    while(at_least_one_change) {
        printf("Iteration: %d\n", iteration);
        at_least_one_change = brushfire_iter(iteration);
        iteration++;
    }

    return iteration;
}
/*
void floodfill() {
    int nvoronoi[IMG_SIZE][IMG_SIZE];
    for (int i = 0; i < IMG_SIZE; i++) {
        for (int j = 0; j < IMG_SIZE; j++) {
            if (voronoi[i][j]==1)       nvoronoi[i][j]=-1;
            nvoronoi[i][j]=0;
        }
    }
    int change=1;

    while(change){
        for (int i = 0; i < IMG_SIZE; i++) {
            for (int j = 0; j < IMG_SIZE; j++) {
                if (nvoronoi==-1) { // not yet reached

                    if (nvoronoi[i+1][j+1]==-1){ // if bottom right neighbour is voronoi
                        //go
                        nvoronoi[i+1][j+1]=nvoronoi[i][j]+1;
                    } else if (nvoronoi[i][j+1]==-1){ // if right neighbour is voronoi
                        //go
                        nvoronoi[i][j+1]=nvoronoi[i][j]+1;
                    } else if
                }
            }
        }
    }
}*/

void drive(){
    VWSetPosition(200,200,0);
    int targetx=3600;
    int targety=3600;

    int to_real=4000/128;
    int to_map=128/4000;

    int x,y,phi;
    VWGetPosition(&x,&y,&phi);

    int tx=targetx-x;
    int ty=targety-y;
    int disp=sqrt(tx*tx+ty*ty);

    while (disp>100){
        int x_map=x*to_map; //current pos on map
        int y_map=y*to_map;

        for (int i=-1;i<2;i++){
            for (int j=-1;j<2;j++){
                if (i==0 && j==0) {
                    //nothing
                }
                else if (voronoi[x_map+i][y_map+j]==1){
                    //calc real disp to target
                    tx=targetx-((x_map+i)*to_real);
                    ty=targety-((y_map+j)*to_real);
                    int new_disp=sqrt(tx*tx+ty*ty);

                    if (new_disp<disp){
                        disp=new_disp;
                        int dx=(x_map+1)*to_real; //real dist to drive x
                        int dy=(y_map+1)*to_real; //real dist to drive y
                        int dist=sqrt(dx*dx+dy*dy); //total dist to drive
                        int angle=atan2(dy,dx);
                        int turn=-phi-90+angle; //angle to turn

                        VWTurn(turn,100);
                        VWWait();
                        VWStraight(dist,100);
                        VWWait();
                    }
                }
            }
        }
    }
}










int main (int argc, char **argv)
{
    int n_iter;         // number of iterations in brushfire
    int n_obstacles;    // number of distinct regions (obstacles)

    LCDMenu("Next","","","");

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
        printf("ERROR: Invalid argument\n");
        printf("Valid options are:\n");
        printf("1: rectangles\n");
        printf("2: fourblocks\n");
        printf("3: u\n");
        return FAIL;
    }

    // Display image
    LCDArea(0, 0, IMG_SIZE, IMG_SIZE, 0xFFFFFF, 1);
    LCDImageStart(0, 0, IMG_SIZE, IMG_SIZE);
    LCDImageBinary(img);
    //KEYWait(KEY1);

    // Convert to a two dimensional array and initialise distances array with obstacles
    for (int i = 0; i < IMG_SIZE; i++) {
        for (int j = 0; j < IMG_SIZE; j++) {
            if (img[i*IMG_SIZE + j] == 1) {
                map[i][j] = true;
                labels[i][j] = 1;
            }
        }
    }

    // Identify obstacles, update colours array
    n_obstacles = find_obstacles();
    printf("\nNumber of obstacles: %d\n", n_obstacles);

    // Show obstacles in colour on LCD
    for (int i = 0; i < IMG_SIZE; i++) {
        for (int j = 0; j < IMG_SIZE; j++) {
            if (colours[i][j] > 0) {
                LCDPixel(IMG_SIZE + 10 + j, i, colours[i][j]);
            }
        }
    }

    // Brushfire
    n_iter = brushfire();
    printf("Brushfire executed in %d iterations\n", n_iter);

    // Print Brushfire output
    for (int i = 0; i < IMG_SIZE; i++) {
        for (int j = 0; j < IMG_SIZE; j++) {
            if (colours[i][j] > 0) {
                LCDPixel(2*(IMG_SIZE + 10) + j, i, colours[i][j]);
            }
        }
    }

    // Print Voronoi points
    for (int i = 0; i < IMG_SIZE; i++) {
        for (int j = 0; j < IMG_SIZE; j++) {
            if (voronoi[i][j]) {
                LCDPixel(j, IMG_SIZE + 10 + i, RED);
                LCDPixel(j, i, RED);
            }
        }
    }

    printf("do the drive\n");
    drive();

    // Check map and labels on console
    // for (int i = 0; i < IMG_SIZE; i++) {
    // 	for (int j = 0; j < IMG_SIZE; j++) {
    //         printf("%d", labels[i][j] % 10);
    //     }
    //     printf("\n");
    // }
    // for (int i = 0; i < IMG_SIZE; i++) {
    // 	for (int j = 0; j < IMG_SIZE; j++) {
    //         printf("%d", map[i][j]);
    //     }
    //     printf("\n");
    // }

    KEYWait(KEY1);

    // Drive




    return 0;
}