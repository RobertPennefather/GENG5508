/* GENG5508 Robotics
 * Semester 2, 2017
 * Lab 5
 * Aaron Hurst, 21325887
 * Gwendolyn Foo, 21299545
 */

#include <stdio.h>		    // printf
#include <math.h>		    // pow, sqrt
#include "eyebot.h"		    // EyeBot 7 library

#define PI			3.14159265

#define MAX_NODES   20
#define MAX_PATHS   200      // expected maximum number of paths (20 nodes each connected to 5 others)
#define MAX_DIST    9999    // set for dead ends in graph

#define GOAL_SIZE   20
#define SMALL_ANGLE 5
#define LARGE_ANGLE 15
#define SPEED	    100
#define WAIT_TIME   200

#define SUCCESS     1
#define FAIL        0

// GLOBALS: graph information
int nodes [MAX_NODES][3];                   // node positions and Euclidean distance to goal
bool connectivity [MAX_NODES][MAX_NODES];   // connectivity between nodes
int distance [MAX_NODES][MAX_NODES];        // distances between nodes
int n_nodes;                                // number of nodes (indexed from 1)

// GLOBALS: A-star
int paths_arry[MAX_NODES][MAX_PATHS];       // paths generated in A-star
int paths_ptr[MAX_PATHS];                   // pointers to current path endpoints (indexed from 0)
int paths_dist[MAX_PATHS];                  // path distances
int n_paths;                                // number of paths

// GLOBALS: drive path
int drive_path_x[MAX_NODES];   // x values of selected path
int drive_path_y[MAX_NODES];   // y values of selected path


//======= read_graph_data
/* Parses nodes text file and stores node information
 * 
 * Node locations are recorded in nodes array, connectivity information
 * if recorded in the (symmetric) connectivity array as boolean values.
 * 
 * return: number of nodes on success, 0 on failure
 */
int read_graph_data(char const* const fileName)
{
    char line[256];
    char *token;
    int n = 0, counter = 0;

    // Open file
    FILE* file_ptr = fopen(fileName, "r");
    if(file_ptr == NULL) {
        perror("Error opening file");
        return 0;
    }
    
    // Get the next line from the file
    while (fgets(line, sizeof(line), file_ptr))
    {
        token = strtok(line, " ");  // split string at " "
        while (token != NULL)
        {
            // Store data based on position in line
            if (counter < 2) {
                nodes[n][counter] = atoi(token);
            }
            else {
                connectivity[n][atoi(token)-1] = true;
            }

            // Update token and counter
            token = strtok(NULL, " ");
            counter++;
        }
        counter = 0;    // reset counter
        n++;            // increment line number
    }

    return n;   // return number of lines (i.e. number of nodes)
}

//======= calc_distances
/* Calculates node-goal and inter-node distances
 */
void calc_distances(void)
{
    int dist_tmp;
    
    // Initialise connectivity array diagonal
    for (int i = 0; i < n_nodes; i++) {
        connectivity[i][i] = true;
    }

    // Calculate inter-node distances for pairwise connected nodes
    for (int i = 0; i < n_nodes; i++) {
        for (int j = 0; j < n_nodes; j++) {
            if (connectivity[i][j]) {
                dist_tmp = (int) sqrt(pow(nodes[i][0]-nodes[j][0],2) + pow(nodes[i][1]-nodes[j][1],2));
                distance[i][j] = dist_tmp;
                distance[j][i] = dist_tmp;
            }
            else {
                distance[i][j] = -1;
                distance[j][i] = -1;
            }
        }
    }

    // Calculate node Euclidean distances to goal
    for (int i = 0; i < n_nodes; i++) {
        dist_tmp = (int) sqrt(pow(nodes[i][0]-nodes[n_nodes-1][0],2) + pow(nodes[i][1]-nodes[n_nodes-1][1],2));
        nodes[i][2] = dist_tmp;
    }
}

//======= print_info
/* Prints graph information
 * Node information, connectivity matrix and inter-node distance matrix
 */
void print_info(void)
{
    // Print node information
    printf("==========================================\n");
    printf("Node information:\n");
    for (int i = 0; i < n_nodes; i++) {
        printf("Node %2d: (%4d,%4d), goal distance: %5d\n", i+1, nodes[i][0], nodes[i][1], nodes[i][2]);
    }
    printf("==========================================\n");

    // Print connectivity matrix
    printf("==========================================\n");
    printf("Connectivity Matrix:\n");
    for (int i = 0; i < n_nodes; i++) {
        for (int j = 0; j < n_nodes; j++) {
            printf("%d ", connectivity[i][j]);
        }
        printf("\n");
    }
    printf("==========================================\n");

    // Print node distance matrix
    printf("==========================================\n");
    printf("Node Distances Matrix:\n");
    for (int i = 0; i < n_nodes; i++) {
        for (int j = 0; j < n_nodes; j++) {
            printf("%5d ", distance[i][j]);
        }
        printf("\n");
    }
    printf("==========================================\n");
}


//TODO: description
void astar_init(void)
{
    int start_node = 1;
    paths_arry[0][0] = start_node;    // one path including only the start node
    paths_dist[0] = nodes[start_node-1][2];  // lower bound goal distance
    paths_ptr[0] = 0;
    n_paths = 1;
}

//TODO: description
//get_dist(old distance, old end point, new end point); //TODO
int get_dist(int dist_cur, int old, int new)
{
    // printf("   %4d\n", dist_cur);
    // printf(" - %4d\n", nodes[old-1][2]);
    // printf(" + %4d\n", distance[old-1][new-1]);
    // printf(" + %4d\n", nodes[new-1][2]);
    // printf("-------\n");
    // printf("   %4d\n", dist_cur - nodes[old-1][2] + distance[old-1][new-1] + nodes[new-1][2]);
    return dist_cur - nodes[old-1][2] + distance[old-1][new-1] + nodes[new-1][2];
}

//TODO: description
//void new_path(n_paths, original path number p, length of original path, new endpoint)
void new_path(int n, int p_old, int len, int end)
{
    // Copy original path new path
    //printf("Copying path %d to new path %d\n", p_old, n);
    for (int i = 0; i <= len; i++) {
        paths_arry[i][n] = paths_arry[i][p_old];
    }
    
    // Add new endpoint
    paths_arry[len+1][n] = end;
}

//TODO: description
//p = path number
void expand_path(int p)
{
    bool skip = false;
    int n_new_paths = 0;                // number of new paths discovered during extension
    int p_ptr = paths_ptr[p];           // path initial endpoint pointer (= path length - 1)
    int p_dist = paths_dist[p];         // path initial goal distance
    int p_end = paths_arry[p_ptr][p];   // path initial endpoint node
    //printf("\np_ptr = %d; p_end = %d\n", p_ptr, p_end);

    // Loop over connectivity matrix to find nodes connected to the initial endpoint
    // Check that the connected nodes are not already in the path
    // Then append path or create a new one
    // ASSUMES: no dead ends (not sure how it will behave in this case)
    for (int i = 1; i <= n_nodes; i++)   // loop through connectivity matrix
    {
        if (connectivity[i-1][p_end-1] && (i != p_end))     // nodes (i) connected to initial endpoint node
        {
            // Loop through path, check if node i is already in it
            skip = false;
            for (int j = 0; j < p_ptr; j++) {
                if (paths_arry[j][p] == i) {
                    skip = true;
                    break;   // break if node i already in path
                }
            }
            
            // Node i not yet in path, extend path
            // Add new endpoint to paths array/add new path including new endpoint
            // Update/record path distance
            // Increment/record path pointer
            // Increment number of new paths added
            if (n_new_paths == 0 && !skip)   // amend initial path
            {
                //printf("Appending path %d with node %d\n", p, i);
                paths_arry[p_ptr+1][p] = i;  // apend old path with new endpoint
                paths_dist[p] = get_dist(p_dist, p_end, i);
                paths_ptr[p]++;
                n_new_paths++;
            }
            else if (!skip)
            {
                //printf("Creating new path from path %d with new end node %d\n", p, i);
                new_path(n_paths, p, p_ptr, i);  // create new path
                paths_dist[n_paths] = get_dist(p_dist, p_end, i);
                paths_ptr[n_paths] = p_ptr + 1;
                n_new_paths++;      // increment number of 
                n_paths++;          // increment number of paths
            }  
        }
    }

    // If path could not be expanded, it is a dead end, so set its distance to a very large value
    if (n_new_paths == 0) {
        paths_dist[p] = MAX_DIST;
    }
}

//TODO: description
// find the shortest path
// returns path number: indexed from zero
int shortest_path(void)
{
    int min_dist = 10000, p_short;
    for (int i = 0; i < n_paths; i++)
    {
        if (paths_dist[i] < min_dist) {
            min_dist = paths_dist[i];
            p_short = i;
        }
    }

    return p_short;
}

//TODO: description
// given the shortest path, check if it ends with the goal node, if so report done
//p = path number
bool as_done(int p)
{
    int p_end = paths_arry[paths_ptr[p]][p];
    if (p_end == n_nodes) {
        return true;
    }
    
    return false;
}


//TODO: description
void as_print()
{
    printf("==========================================================================\n");
    printf("CURRENT NUMBER OF PATHS: %d\n", n_paths);
    printf("CURRENT PATHS:\n");
    printf("paths                                                         dist:   ptr:\n");
    for (int i = 0; i < n_paths; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
            printf("%2d ", paths_arry[j][i]);
        }
        printf("   %4d     %2d\n", paths_dist[i], paths_ptr[i]);
    }
    printf("==========================================================================\n");
}

//======= drive_to_pt
/* Drives the robot toward a specified target point (x,y)
 * 
 * Manages rotation to align with waypoint direction, maintains robot on course
 * using VWCurve if required. Output is printed as robot drives.
 * Assumes coordinate system has been set correctly (i.e. does not use VWSetPosition)
 * Returns when the robot reaches the target point.
 */
int drive_to_pt(int x, int y)
{
	// Calculate heading to target, maintain this heading
	int x_cur, y_cur, phi_cur;
    float target_dist, target_angle;

    // Print target information
    printf("==========================================\n");
    printf("TARGET OBTAINED\n");
    printf("Target position:  (%5d, %5d)\n", x, y);
    printf("==========================================\n");

    // Calculate relative position of target
    VWGetPosition(&x_cur, &y_cur, &phi_cur);
    target_dist = (int) sqrt(pow(x-x_cur,2) + pow(y-y_cur,2));
    target_angle = (int) 180*atan2(y-y_cur, x-x_cur)/PI - phi_cur;
    
    // Drive to target
    while (target_dist > GOAL_SIZE)
    {
        // Print state information
        printf("==========================================\n");
        //printf("Current position: (%5d, %5d)\n", x_cur, y_cur);
        //printf("Target position:  (%5d, %5d)\n", x, y);
        printf("Target distance:  %5.1f\n", target_dist);
        printf("Target angle:     %5.1f\n", target_angle);
        printf("==========================================\n");
        
        // Control
        if (abs(target_angle) > LARGE_ANGLE) {
            VWTurn(target_angle, SPEED);
            printf("---- ACTION: rotating %4.1f degrees ----\n", target_angle);
            VWWait();
        }
        else if (abs(target_angle) < SMALL_ANGLE) {
            VWStraight(100, SPEED);
            printf("---- ACTION: driving straight ----\n");
        }
        else if (target_angle < 0) {
            VWCurve(150, -30, SPEED);
            printf("---- ACTION: turning right ----\n");
        }
        else if (target_angle > 0) {
            VWCurve(150, 30, SPEED);
            printf("---- ACTION: turning left -----\n");
        }
        OSWait(WAIT_TIME);
        
        // Calculate relative position of target
        VWGetPosition(&x_cur, &y_cur, &phi_cur);
        target_dist = (int) sqrt(pow(x-x_cur,2) + pow(y-y_cur,2));
        target_angle = (int) 180*atan2(y-y_cur, x-x_cur)/PI - phi_cur;
    }

    // Print state information
    printf("==========================================\n");
    printf("==========================================\n");
    printf("TARGET REACHED\n");
    printf("Current position: (%5d, %5d)\n", x_cur, y_cur);
    printf("Target position:  (%5d, %5d)\n", x, y);
    printf("Target distance:  %4.1f\n", target_dist);
    printf("==========================================\n");

    return SUCCESS;
}


/*************************************************************************************
 * MAIN
 ************************************************************************************/
int main (int argc, char **argv)
{
    LCDMenu("NEXT","","","EXIT");
    int ret;

    // Read graph data
    if (argc < 2)
    {
        printf("\nERROR: You must specify which file to load graph data from as a command line argument.\n");
        printf("Available options are:\n");
        printf("1: branch.txt\n");
        printf("2: cycle.txt\n");
        printf("3: grid.txt\n");
        printf("4: nodes.txt\n");
        return FAIL;
    }
    else if (atoi(argv[1]) == 1)	ret = read_graph_data("branch.txt");
    else if (atoi(argv[1]) == 2)	ret = read_graph_data("cycle.txt");
    else if (atoi(argv[1]) == 3)	ret = read_graph_data("grid.txt");
    else if (atoi(argv[1]) == 4)	ret = read_graph_data("nodes.txt");
    else {
        printf("ERROR: Invalid argument\n");
        printf("Valid options are:\n");
        printf("1: branch.txt\n");
        printf("2: cycle.txt\n");
        printf("3: grid.txt\n");
        printf("4: nodes.txt\n");
        return FAIL;
    }

    // Check data read successfully
    if (ret == 0) {
        return FAIL;
    }
    else {
        n_nodes = ret;
        ret = 0;
    }

    // Print network to LCD
    for (int i = 0; i < n_nodes; i++) {
        LCDPixel(nodes[i][0]/5, nodes[i][1]/5, RED);
        for (int j = i+1; j < n_nodes; j++) {
            if (connectivity[i][j]) {
                LCDLine(nodes[i][1]/5, nodes[i][0]/5, nodes[j][1]/5, nodes[j][0]/5, RED);
            }
        }
    }

    // Calculate distances
    calc_distances();

    // Print current information
    print_info();

    KEYWait(KEY1);
    
    // A* Algorithm
    bool done = false;
    int path_cur = 0;
    astar_init();
    as_print();
    //KEYWait(KEY1);
    while (!done) {
        expand_path(path_cur);
        path_cur = shortest_path();
        done = as_done(path_cur);
        as_print();
        if (paths_dist[path_cur] == MAX_DIST)
        {
            printf("There is no path between the start and end nodes.\n");
            return FAIL;
        }
        //KEYWait(KEY1);
    }
    KEYWait(KEY1);
    
    // Path generation
    int path_length = paths_ptr[path_cur] + 1;
    for (int i = 0; i < path_length; i++) {
        drive_path_x[i] = nodes[paths_arry[i][path_cur]-1][0];
        drive_path_y[i] = nodes[paths_arry[i][path_cur]-1][1];
    }

    // Display path
    printf("Path selected using A*:\n");
    for (int i = 0; i < path_length; i++) {
        printf("Point %2d: (%4d,%4d)\n", i+1, drive_path_x[i], drive_path_y[i]);
    }
    for (int i = 1; i < path_length; i++) {
        LCDLine(drive_path_y[i-1]/5, drive_path_x[i-1]/5, drive_path_y[i]/5, drive_path_x[i]/5, BLUE);
    }
    KEYWait(KEY1);

    // Drive shortest path
    VWSetPosition(drive_path_x[0],drive_path_x[1],0);
    for (int i = 1; i < path_length; i++) {
        drive_to_pt(drive_path_x[i], drive_path_y[i]);
    }
    printf("DONE\n");
    KEYWait(KEY4);

    return SUCCESS;
}