// EyeBot Demo Program: Drive, T. Bräunl, Nov. 2015
#include<stdio.h>
#include "eyebot.h"
#include<math.h>

#define Interval 0.1
#define Scaling  1500
#define N         45

void SplineDrive(  int x , int y, int alpha         ){

	//initial position of car
	int px = 0;
	int py = 0;
	
	//initial direction of car
	int dpx = Scaling *  1;
	int dpy = Scaling *  0;

	//final direction of car
	int pkx = x;
	int pky = y;

	//final direction of car
	int dpkx =  Scaling * cos( alpha / 180.0 * M_PI    )  ;
	int dpky =  Scaling * sin( alpha / 180.0 * M_PI    ) ;

	int current_x = px;
	int current_y = py;
	float current_angle =  0;      //?? the starting angle faced?

//	printf("--------------------------------------\n" );
//	printf("dpkx is %d   dpky is %d\n" , dpkx ,  dpky  );
//	printf("x is %d   y is %d\n" , x ,  y  );
//	printf("--------------------------------------\n" );


	for(float u = 0.1; u <= 1.1 ; u += Interval    ){

		float H0 = 2 * (u*u*u) - 3 * ( u*u ) +1;
		float H1 = -2 * (u*u*u) + 3 * ( u*u ) ;
		float H2 = (u*u*u) - 2 * ( u*u ) + u;
		float H3 = (u*u*u) -  ( u*u ) ;

 		float new_x = H0 * px + H1 * pkx + H2 * dpx + H3 * dpkx   ;
 		float new_y = H0 * py + H1 * pky + H2 * dpy + H3 * dpky   ;

		float dH0   = 6 * (u*u)  - 6*(u)    ;
		float dH1   = -6 * (u*u) + 6*(u)    ;
		float dH2   = 3 * (u*u)  - 4*(u) +1 ;
		float dH3   = 3 * (u*u)  - 2*(u)    ;

 		float new_d_x = dH0 * px + dH1 * pkx + dH2 * dpx + dH3 * dpkx   ;
 		float new_d_y = dH0 * py + dH1 * pky + dH2 * dpy + dH3 * dpky   ;

//		printf("--------------------------------------\n" );
//		printf("x is %f   y is %f\n" , new_x ,  new_y  );
		
		
		float new_angle = (atan2( new_d_y , new_d_x )) / M_PI * 180  ;
		float angle_difference = new_angle - current_angle;
		
		if( angle_difference > 180   ){
			angle_difference = - (360 - angle_difference);
		}
		if( angle_difference < -180     ){
			angle_difference = 360 +  angle_difference;			
		}

//		printf("new angle value is %f\n" , new_angle  );
//		printf("old angle value is %f\n" , current_angle  );
//		printf("change angle value is %f\n\n" , angle_difference  );
		
		int x_dis = abs( new_x - current_x  );
		int y_dis = abs( new_y - current_y  );
		int dis   = sqrt( x_dis * x_dis + y_dis * y_dis  ) ;	


                if(     angle_difference < 5 && angle_difference > 0    ){
                        angle_difference = 4;
                }
                if(     angle_difference > -5 && angle_difference < 0   ){
                        angle_difference = -4;
                }
                if(     abs(angle_difference) > 170 && abs(angle_difference) <= 180       ){

                        VWTurn( angle_difference , 100 );
                        VWWait();
                }
                else{
                        VWCurve( dis , angle_difference , 100  );
                        VWWait();
                }







//		printf("--------------------------------------\n\n" );



		current_angle =  new_angle  ;
		current_x     =  new_x      ;		
		current_y     =  new_y      ;
	}



}




int main(){
	
	int waypoints [20][2];

	FILE* file=fopen("way.txt", "r"); // open way.txt to read

  	char line[256]; //create memory space between "    "

  	int index =0;

  	while (	fgets(line, sizeof(line), file)	){ //read each line until end of file

		char* split=strtok (line," "); //split line between spaces ""
    		waypoints[index][0]=atol(split); //convert first part to int, store as x
    		split = strtok (NULL, " "); // get next part after space
    		waypoints[index][1]=atol(split); //convert second part to int, store as y

    		LCDPrintf("%i, %i\n", waypoints[index][0], waypoints[index][1]);
    		index++;

  	}

  	fclose(file);
	int a = 0;

	int global_x = 0 ;
	int global_y = 0 ;

	int local_x = 0 ;
	int local_y = 0 ;


	while(a < 4){

		a = a % 4 ;

		global_x = waypoints[a][0];
		global_y = waypoints[a][1];

		printf( "iteration number %i \n" , a  );
		printf( "new global x value is %i new global y value is %i \n" , global_x , global_y  );

		local_x = global_x - local_x;
		local_y = global_y - local_y;

		int angle = atan2( waypoints[(a+1)%N][1] -global_y  ,  waypoints[(a+1)%N][0]-global_x)/M_PI * 180;	

		printf( "----hahah %i %i \n" ,  waypoints[(a+1)%N][1] -global_y  , waypoints[(a+1)%N][0]-global_x  );
		printf( "new local x value is %i new loacl y value is %i , angle value is %i \n\n\n" , local_x , local_y, angle  );
		
		if(angle == 90){
			angle = -90;
		}

//		SplineDrive( local_x , local_y , angle );
		SplineDrive( local_x , local_y , 0 );

		local_x = global_x   ;
		local_y = global_y   ;

		printf("next iteration\n");
		int b = 0;
		while( b < 1000000000 ){
			b++;	
		}

		a++;
	};
}
