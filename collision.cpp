

/*
    Copyright (C) 2007  Derek Weitzel
    This file is part of Fish.

    Fish is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Fish is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


*/



#ifndef WIN32
#include <GL/glut.h>
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "collision.h"
#include "aquarium.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include "collisionMap.h"


#define PI 3.14159265

extern struct Fish* fish;
extern double bubblerloc[3];
extern int FISHCOUNT;
#define IMMUNETIME 1

//used to draw lines for debugging collision stuff
//#define COLLISIONLINES

//debug stuff
GLfloat linespec[] = { 1.0, 1.0, 1.0, 1.0 };

extern collisionMap* cm;

void collision_avoidance(int fishnum) {
	double dist = 0;
	double angle = 0, totalangle = 0, tempangle = 0;
	int i = 0;
	int numofFish = 0;
	double min = 10000000;  //huge number just to throw off the min
	int indexMin = 0;
	double tempdist = 0;
	
	indexMin = cm->getClosestFish(fishnum);
	//printf("found %i to be closest\n", indexMin);

	
	//fish[fishnum].immune += getTime();
//	min = sqrt(pow(fish[0].location[2] - fish[fishnum].location[2], 2) + pow(fish[0].location[0] - fish[fishnum].location[0], 2));
//	for(i = 0; i < FISHCOUNT; i++) {
	//printf("%i's num is %i\n", fishnum, fish[fishnum].numplaneFish);
/*
	for(i = 0; i < fish[fishnum].numplaneFish; i++) {

		//if(fabs(fish[i].location[1]-fish[fishnum].location[1])<70 && i != fishnum && fish[fishnum].alive == 1) {
		if(fish[fishnum].planeFish[i]->alive == 1) {
			//get distance between the fishies
			//dist = sqrt(pow(fish[i].location[2] - fish[fishnum].location[2], 2) + pow(fish[i].location[0] - fish[fishnum].location[0], 2));
			dist = sqrt(pow(fish[fishnum].planeFish[i]->location[2] - fish[fishnum].location[2], 2) + pow(fish[fishnum].planeFish[i]->location[0] - fish[fishnum].location[0], 2));
				
			//get the min
			if(dist < min) {
				min = dist;
				indexMin = i;
				//printf("Found min of %i with %i, which was %lf\n", fishnum, i, dist);
			}
		}
		else {
			continue;
		}
	}
	
	*/
	
	/*if(fishnum == 1) {
		printf("----------------------------------\n");
		printf("location[0] = %lf\n", fish[fishnum].location[0]);
		printf("location[2] = %lf\n", fish[fishnum].location[2]);
		printf("dist = %lf\n", dist);
		fflush(stdout);
	}
*/	
	//set the angle to the angle from the closest fish to the fish i'm on
//	angle = atan2(fish[indexMin].location[0] - fish[fishnum].location[0], fish[fishnum].location[2] - fish[fishnum].location[2]);
	angle = atan2(fish[indexMin].location[0] - fish[fishnum].location[0], fish[indexMin].location[2] - fish[fishnum].location[2]);
	min = cm->getDistance(indexMin, fishnum);
	
	//walls of fish tank
	if (min > (xmax-fish[fishnum].location[0])){
		angle = PI/2;
		min = xmax-fish[fishnum].location[0];
	}
	if (min > (fish[fishnum].location[0] - xmin)) {
		angle = (3*PI)/2;
		min = fish[fishnum].location[0] - xmin;
	}
	if (min > (zmax - fish[fishnum].location[2])) {
		angle = 0;
		min = zmax - fish[fishnum].location[2];
	}
	if (min > (fish[fishnum].location[2] - zmin)) {
		angle = PI;
		min = fish[fishnum].location[2] - zmin;
	}
	
	//lighthouse test
	tempdist = sqrt(pow((-bubblerloc[0]+20) - fish[fishnum].location[0], 2) + pow(20-fish[fishnum].location[2], 2));
	if (min > tempdist && fish[fishnum].location[1] < ymin+(700)) {
		
		angle = atan2(-bubblerloc[0] - fish[fishnum].location[0], 0-fish[fishnum].location[2]);
		min = tempdist;
		
	}
	
	
	//castle test
	tempdist = sqrt(pow((bubblerloc[0]) - fish[fishnum].location[0], 2) + pow((bubblerloc[2])-fish[fishnum].location[2], 2));
	if (min > tempdist-300 && fish[fishnum].location[1] < ymin+500) {
		angle = atan2(bubblerloc[0] - fish[fishnum].location[0], bubblerloc[2]-fish[fishnum].location[2]);
		min = tempdist;
	}
	

	
		//make the target rotation the opposite way from the closest fish
		//really, the fish are always running from the closest other fish	
			angle += PI;
			fish[fishnum].targetrotation = (180/PI)*angle;

	
/*		if(fishnum == 1) {
		printf("angle = %lf\n", angle);
		printf("dist = %lf\n", dist);
		fflush(stdout);
	}
*/		

	
	
			//if((fish[indexMin].location[0] - fish[fishnum].location[0]) < 0)
				//angle *= -1;
	//		printf("angle = %lf\n", angle);
	//		fflush(stdout);

			//fish[fishnum].rotationspeed = 30/dist;
			
/*			if(fishnum == 1) {
			printf("------------------------------\n");
			printf("target rotation = %lf\n", fish[fishnum].targetrotation);
			printf("rotation = %lf\n", fish[fishnum].rotation);
			fflush(stdout);
			}
*/			/*
			if(fish[fishnum].rotation < 0)
				fish[fishnum].rotation += 360;
			else if(fish[fishnum].rotation > 360)
				fish[fishnum].rotation -= 360;
				
			if(fish[fishnum].rotation > 180)
				tempangle = fish[fishnum].rotation - 360;
			else
				tempangle = fish[fishnum].rotation;
				
			totalangle = angle - (PI/180)*tempangle;
			totalangle = totalangle * (180/PI);
			
			if(fishnum == 0) {
			printf("-------------------------------------\n");
			printf("tempangle = %lf\n", tempangle);
			printf("rotation = %lf\n", fish[fishnum].rotation);
			printf("total angle = %lf\n", totalangle);	
			fflush(stdout);
			}
			if(totalangle < -90) {
				fish[fishnum].rotation += 1.5;
			} else if(totalangle < -45) {
				fish[fishnum].rotation += 1.5;
			} else if(totalangle < 0) {
				fish[fishnum].rotation += 1.5;
			} else if(totalangle < 45) {
				fish[fishnum].rotation -= 1.5;
			} else if(totalangle < 90) {
				fish[fishnum].rotation -= 1.5;
			} else if(totalangle >= 90) {
				fish[fishnum].rotation -= 1.5;
			}
			
			*/
	
			//only drawn if collisionlines is defined
			#ifdef COLLISIONLINES
			glPushMatrix();
			
			GLfloat shiny = 1.0;
			 glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, linespec);
			  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, linespec);
			  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, linespec);
			  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, linespec);

			
			glBegin(GL_LINES);
				glVertex3d(fish[fishnum].location[0], fish[fishnum].location[1], fish[fishnum].location[2]);
				glVertex3d(fish[indexMin].location[0], fish[indexMin].location[1], fish[indexMin].location[2]);
				
			glEnd();
			

			glTranslated(fish[fishnum].location[0], fish[fishnum].location[1], fish[fishnum].location[2]);
			glRotated((180/PI)*angle, 0.0, 1.0, 0.0);
			glBegin(GL_LINES);
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(0.0, 0.0, 100.0);
			glEnd();
			glPopMatrix();
			#endif //COLLISIONLINES
			
			//find the angle between 2 fish


/*			printf("---------------------------\n");
			printf("angle = %lf\n", (180/PI)*angle);
		//	angle += PI;
			printf("angle after += pi = %lf\n", (180/PI)*angle);
		//	if(angle < 0) 
		//		angle += 2*PI;
			printf("angle after if = %lf\n", (180/PI)*angle);
		//	angle = fmod(angle, 2* PI);
			printf("angle after fmod = %lf\n", (180/PI)*angle);
			totalangle += (180/PI)*angle;
			totaldistance += dist;
			
			
			
			
			printf("fishnum = %i\n", numofFish);
			printf("angle between = %lf\n", (180/PI)*acos((fish[i].location[0] - fish[fishnum].location[0])/dist));
			printf("angle = %lf\n", (180/PI)*angle);
	*/	//	printf("total angle = %lf\n", totalangle);
			//angle between facing the fish
			//angle = angle - fmod((PI/180)*fish[fishnum].rotation, 2*PI);
			//debug stuff

			// not working correctly
/*				fish[fishnum].targetrotation += ((-1)*(.5)*tan((180*PI)*angle/2+(PI/2)))*(40/(dist - (fish[i].size + fish[fishnum].size)*34));
				fish[fishnum].targetrotation = fmod(fish[fishnum].targetrotation, 360);
				glTranslated(fish[fishnum].location[0], fish[fishnum].location[1], fish[fishnum].location[2]);
				glRotated(fish[fishnum].targetrotation, 0.0, 1.0, 0.0);
				
				
				glBegin(GL_LINES);
					glVertex3d(0.0,0.0,0.0);
					glVertex3d(100,fish[fishnum].location[1],100);
				glEnd();
	*/			


				
		//		fish[fishnum].targetrotation += (angle+PI) * (1/(dist - (fish[i].size + fish[fishnum].size)*34));
				//fish[fishnum].targetrotation -= (180/PI)*(angle+2*PI)*(1/dist) + (fmod(rand(), 200)-100)/2;
				
				fish[fishnum].immune = 0;
				/*
				printf("angle difference = %lf\n", (180/PI)*angle);
				printf("rotation before = %lf\n", fish[fishnum].rotation);
				printf("tan = %lf\n", 1/(tan(angle/2 + PI)));
				printf("distarg = %lf\n", 20/(dist - (fish[i].size + fish[fishnum].size)*34));
				printf("current angle = %lf\n", fish[fishnum].rotation);
				printf("target angle = %lf\n", fish[fishnum].targetrotation);
				printf("dist = %lf\n", dist);
			*/
	//			printf("num of fish = %i", numofFish);
			//		fflush(stdout);
				
			
		
	

}
			/*
			//get the angle between them
			angle = atan((fish[i].location[0] - fish[fishnum].location[0])/(fish[i].location[2] - fish[fishnum].location[2]));
			//calculate the angle between the direction the fish is facing and the position of the other fish
			angle = (PI/180.0)*fish[fishnum].rotation - angle;
			
			//angle now holds the angle between the direction the fish is facing and the location of the other fish
			
			//change the angle of the fish movement
			
			if(abs(fish[i].location[1] - fish[fishnum].location[1]) < 70) {
				fish[fishnum].rotation += (1/((dist-((fish[i].size + fish[fishnum].size)*34.0))/2))*(1/fmod(angle, 360));
			#ifdef COLLISIONDEBUG
				if(fishnum == 1) {
				printf("fish = %i ", i);
				printf("distance = %lf ", dist);
				printf("angle = %lf\n", angle);
				//printf("rotation = %lf\n", fish[fishnum].rotation);
				//printf("changed = %lf\n", (3/(dist-((fish[i].size + fish[fishnum].size)*34.0)))*(cos(angle)+1));
				fflush(stdout);
				}
			#endif //COLLISIONDEBUG
				
			}
		}
		
			
	}
	//insert code for walls, castle, and lighthouse
	
	//front wall

	dist = fabs(fish[fishnum].location[2]-450.0);
	angle = fish[fishnum].rotation;
	fish[fishnum].rotation += (10/((dist-(fish[fishnum].size)*34.0)/2))*(1/fmod(angle, 360));

/*	if((1/(dist-(fish[fishnum].size)*34.0))*(180/PI)*(tan((angle*PI/180)/2+PI/2)) > 20) {
		printf("\nchanged = %lf\n", (1/(dist-(fish[fishnum].size)*34.0))*(180/PI)*(tan((angle*PI/180)/2+PI/2)));
		printf("angle(rad) = %lf\n", fish[fishnum].rotation*PI/180);
		printf("x = %lf, z = %lf\n", fish[fishnum].location[0], fish[fishnum].location[2]);
		printf("size = %lf\n", fish[fishnum].size);
		printf("dist = %lf\n", dist); 
		fflush(stdout);
		exit(0);
		
			
	}
*/	/*
	printf("zmax = %i ", zmax);
	printf("current x = %lf\n", fish[fishnum].location[2]);
	fflush(stdout);
	*/
	/*dist = abs(fish[fishnum].location[2]-zmin);
	angle = fish[fishnum].rotation-180;
	fish[fishnum].rotation += (10/((dist-(fish[fishnum].size)*34.0)/2))*(1/fmod(angle,360));
	*/
