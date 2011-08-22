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


Author: Derek Weitzel

*/

#ifndef WIN32
#include <sys/time.h>
#include <GL/glut.h>
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <stdio.h>
#include "aquarium.h"
#include "fishmodel.h"
#include "fishStats.h"
#include "shaders.h"

extern Fish* fish;
extern int FISHCOUNT;
extern bool picking;
extern double fov;
extern int SleepTime;
extern fishStats fishstats;
extern int cameraCounter;
extern int cameraSelected;
extern bool nightTime;
#ifdef WIN32
extern HWND hWnd;
#endif
void begin_picking(int x, int y) {

	picking = true;

	glPushMatrix();
	GLuint buffer[100];
	GLint viewport[4];
	
		glGetIntegerv(GL_VIEWPORT,viewport);
	
	glSelectBuffer(100, buffer);
	glRenderMode(GL_SELECT);


	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	
	gluPickMatrix(x, viewport[3]-y, 10, 10, viewport);
#ifndef WIN32
	gluPerspective(fov, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 40, 5000.0);
#else
	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);
	gluPerspective(fov, (float)(windowRect.right-windowRect.left)/(float)(windowRect.bottom-windowRect.top), 40, 5000.0);
#endif



	glInitNames();
	int temp = SleepTime;
	SleepTime = 0;

	glMatrixMode(GL_MODELVIEW);
	 display();
	 
	 	

		for(int a = 0; a < FISHCOUNT; a++) {
		fish[a].picked = false;	
		
	}
	SleepTime = temp;
	int hits = glRenderMode(GL_RENDER);
		 	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	int fishChange;
	if(hits)
		fishChange = buffer[3];
	else
		fishChange = -1;
	unsigned int min = buffer[1];
	int i = 0;
	int checked = 0;
/*	printf("hits = %i\n", hits);
	printf("buffer0 = %i\n", buffer[0]);
	printf("buffer1 = %i\n", buffer[1]);
	printf("buffer2 = %i\n", buffer[2]);
	printf("buffer3 = %i\n", buffer[3]);
	printf("buffer4 = %i\n", buffer[4]);
	printf("buffer5 = %i\n", buffer[5]);
	printf("buffer6 = %i\n", buffer[6]);
	printf("buffer6 = %i\n", buffer[7]);
	printf("buffer6 = %i\n", buffer[8]);
	printf("buffer6 = %i\n", buffer[9]);
	if(buffer[3] < FISHCOUNT && buffer[3] > 0)
	printf("side = %s\n", fish[buffer[3]].print);
*/	
	while(checked < hits) {
		if(buffer[i] == 0) {
//			printf("0 hits = %i\n", buffer[i]);
			checked++;
			i += 2;
			continue;
		}
		i++;
		if(min > buffer[i]) {
//			printf("found min = %i, to %i\n", buffer[i], buffer[i+2]);
//			fflush(stdout);
			min = buffer[i];
			i+=2;
			fishChange = buffer[i];
			i++;
			checked++;
		} else {
		i+=3;
		checked++;
		}
	}

	if(fishChange < FISHCOUNT && fishChange >= 0) {
		cameraCounter = 0;
//	printf("value on side = %s\n", fish[fishChange].print);
	cameraSelected = 0;
	if(fishChange != -1) {
	fish[fishChange].picked = true;
	
	}
	fishstats.selectFish(fishChange);
	} else {
		cameraSelected = 0;
		fishstats.selectFish(-1);
	}
	if(fishChange == FISHCOUNT+1) {
		//fishstats.selectFish(fishChange);
		cameraSelected = CASTLESELECTED;
		
	} else if(fishChange == FISHCOUNT+2) {
		//lighthouse is selected
		if(!nightTime) {
			nightTime = true;
			nightShaders();
		}
		else {
			nightShaders();
			nightTime = false;
		}
	}
	//GLint viewport[4];




       
       glPopMatrix();	
	
	picking = false;
	
	
	
}

