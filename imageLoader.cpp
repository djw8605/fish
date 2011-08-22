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

#include <stdlib.h>
#include <iostream>

#ifndef WIN32
#include <sys/time.h>
#include <GL/glut.h>
#include <pthread.h>
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


using namespace std;

#include "CTargaImage.h"
#include "aquarium.h"


struct texName {
	unsigned int* Tex;
	char* nodeName;
	Fish* fish;
};

unsigned int tex;
bool done = true;
CTargaImage image;

void* image_start(void* Tex) {

#ifndef WIN32
	pthread_detach(pthread_self());
	texName* ten = (texName*)Tex;
	
	//get the image from the website, should be implemented with config file
	string temp = "nice -n 10 curl \"";
	temp.append(ten->fish->clusterPtr->website);
	temp.append("graph.php?g=load_report&z=medium&c=");
	temp.append(ten->fish->fishcluster);
	temp.append("&h=");
	temp.append(ten->nodeName);
	temp.append("&m=&r=day&s=descending&hc=4\" > /tmp/graph.png");
	
	/* stop getting the image if it can't be downloaded */
	if (system(temp.c_str())) {
		return 0;
	}
	usleep(10000);
	system("nice -n 10 convert /tmp/graph.png /tmp/graph.tga");
	
	system("rm /tmp/graph.png");
	

	bool error = image.Load("/tmp/graph.tga");
	if(!error) {
		//printf("error loading file\n");
		done = true;
		image.Release();
		delete ten;
		return 0;
	} else
		done = false;
	while(!done)
		usleep(1000);
		
	image.Release();
	*ten->Tex = tex;
	delete ten;
	system("rm /tmp/graph.tga");

#endif
	return 0;
}



void image_incriment() {
	
	if(!done) {
			
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &tex);
	
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.GetWidth(), image.GetHeight(),
//					0, GL_RGB, GL_UNSIGNED_BYTE, image.GetImage());
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image.GetWidth(), image.GetHeight(),
					GL_RGBA, GL_UNSIGNED_BYTE, image.GetImage());
					
	done = true;
	
	}
	
}


















void request_image(unsigned int* Tex, char* nodeName, Fish* fish) {
	
	
	if(!Tex)
		return;
		
	if(tex)
		glDeleteTextures(1, &tex);
		
#ifndef	WIN32
	pthread_t imageThread;
	texName* ten = new texName;
	ten->Tex = Tex;
	ten->nodeName = nodeName;
	ten->fish = fish;
	pthread_create(&imageThread, 0, image_start, ten);
#endif
//	image_start(ten);
	
	
	
}
