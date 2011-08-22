
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
#include <time.h>
#endif

#include <stdio.h>

#include "aquarium.h"
#include "FreeType.h"
#include "fishStats.h"
#include "shaders.h"

#define MENUSPEED 75;
#ifndef WIN32
extern freetype::font_data font;
#endif

extern Fish* fish;
extern int statFields;
extern Stats stats;

fishStats::fishStats() {
	fishnum = -1;
	oldfishnum = -1;
	graphTex = 0;
}

fishStats::~fishStats() {


}



void fishStats::selectFish(int fishnum) {
#ifndef WIN32
	
	this->fishnum = fishnum;
		if(fishnum != -1) {
	this->oldfishnum = fishnum;
	if(glIsTexture(graphTex)) {
		glDeleteTextures(1, &graphTex);
	}
	if(fish[fishnum].clusterPtr->website) {
		request_image(&graphTex, fish[fishnum].fishname, fish+fishnum);
	}
		}
		if(fishnum == -1) {
			if(graphTex)
			glDeleteTextures(1, &graphTex);
			graphTex = 0;
		}
	
	
#endif
}

void fishStats::render() {
#ifndef WIN32
	static double counter = 0;
		if(counter < 100 && fishnum != -1) {
		counter += getTime() * MENUSPEED; 
	} else if (fishnum == -1 && counter > 0) {
		counter -= getTime() * MENUSPEED;
	}

	if(counter > 0) { 
	
	glPushMatrix();
	pushShader();	
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);	
	
	

		
	glTranslatef(glutGet(GLUT_WINDOW_WIDTH)-counter*3, 0, 0);
	
	glColor4f(0.0, 0.0, 1.0, 1.0);
	
	int height = glutGet(GLUT_WINDOW_HEIGHT)-30;
	
	glBegin(GL_QUADS);
		glVertex2f(0.0, height-(statFields+2)*30);
		glVertex2f(0.0, height);
		glVertex2f(300, height);
		glVertex2f(300, height-(statFields+2)*30);
	glEnd();
	
	if(graphTex) {
		glEnable(GL_TEXTURE_2D);
		glColor4f(1.0, 1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, graphTex);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex2f(0.0, (height-(statFields+2)*30)-100);
		glTexCoord2f(0.0, 1.0); glVertex2f(0.0, height-(statFields+2)*30);
		glTexCoord2f(1.0, 1.0); glVertex2f(300.0, height-(statFields+2)*30);
		glTexCoord2f(1.0, 0.0); glVertex2f(300.0, (height-(statFields+2)*30)-100);
	glEnd();
		
		
	}

	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
	
	
	
	glColor4f(0.0, 0.0, 1.0, 1.0);
//	printf("before oldfishnum\n");
//	fflush(stdout);
	if(fishnum != -1) {
		int i = 0, c = 0, count = 0;
		freetype::print(font, 20, height-20, "Node = %s", fish[fishnum].fishname);
		freetype::print(font, 20, height-50, "Cluster = %s", fish[fishnum].fishcluster);
		for(i = 0; i < statFields; i++) {
				freetype::print(font, 20, height-(80+30*i), "%s:  %s", stats.names[i], fish[fishnum].nodeinfo.info[i]);
				
		}
	}
//		printf("after oldfishnum\n");
//	fflush(stdout);
		popShader();
		glPopMatrix();
	}
#endif	
}


