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


Authors: Cory Lueninghoener, Derek Weitzel
*/


#include "../CTargaImage.h"
#ifndef	WIN32
#include <GL/glut.h>
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include "partialcylinder.h"
#include "solidRectangle.h"
#include "castlemodel.h"


#define BRIDGEINDICES 4

GLuint texture, drawbridgeTex, towerTex;
GLuint list;

	GLfloat vertBridgeArray[] = {
								0.0, 0.0, 0.0,
								0.0, 10.0, 100.0,
								100.0, 10.0, 100.0,
								100.0, 0.0, 0.0
	
};

GLfloat texBridgeArray[] = {
								0.0, 0.0,
								0.0, 1.0, 
								1.0, 1.0, 
								1.0, 0.0
	
};



int slices = 100;
void draw_frontWall(float length, float width, float height);
void init_castle()
{
  GLUquadricObj *qobj; 
  
  list = glGenLists(7);
  qobj = gluNewQuadric();
  
  CTargaImage image2;
  if(image2.Load("./wall.tga")) {

  glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.GetWidth(), image.GetHeight(),
//					0, GL_RGB, GL_UNSIGNED_BYTE, image.GetImage());
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image2.GetWidth(), image2.GetHeight(),
					GL_RGB, GL_UNSIGNED_BYTE, image2.GetImage());
			
  image2.Release();
  }
  if(image2.Load("./drawbridge.tga")) {
  
   glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &drawbridgeTex);
	
	glBindTexture(GL_TEXTURE_2D, drawbridgeTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.GetWidth(), image.GetHeight(),
//					0, GL_RGB, GL_UNSIGNED_BYTE, image.GetImage());
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image2.GetWidth(), image2.GetHeight(),
					GL_RGB, GL_UNSIGNED_BYTE, image2.GetImage());
			
  image2.Release();
  }
  if(image2.Load("./wallthin.tga")) {
  glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &towerTex);
	
	glBindTexture(GL_TEXTURE_2D, towerTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image2.GetWidth(), image2.GetHeight(),
					GL_RGB, GL_UNSIGNED_BYTE, image2.GetImage());
			
  image2.Release();
  }
					
 glBindTexture(GL_TEXTURE_2D, texture);
  gluQuadricDrawStyle(qobj, GLU_FILL);
  gluQuadricNormals(qobj, GLU_SMOOTH);
  gluQuadricTexture(qobj, true);
  /*cylinders */
  glNewList(list, GL_COMPILE);
  glBindTexture(GL_TEXTURE_2D, texture);
  	gluCylinder(qobj, 15.0, 15.0, 100.0, 15, 1);
  glEndList();
  /*walls*/
  glNewList(list+1, GL_COMPILE);



  	glBindTexture(GL_TEXTURE_2D, texture);
 	solidRectangle(90.0,20.0,70.0, 1.0, 1.0);

  // glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glEndList();
  /* blocks */
  glNewList(list+2, GL_COMPILE);
  glBindTexture(GL_TEXTURE_2D, texture);
    solidRectangle(5.0,5.0,5.0, .02, .02);
  glEndList();
  /*tower tops */ 
  glNewList(list+3, GL_COMPILE);
  glBindTexture(GL_TEXTURE_2D, towerTex);
    gluCylinder(qobj, 20.0, 20.0, 7.0, 15, 1);
  //partialcylinder(0.0,20.0,0.0,360.0,7.0,slices);
  glEndList();
  /*tower tops 2 */
  glNewList(list+4, GL_COMPILE);
  glBindTexture(GL_TEXTURE_2D, towerTex);
    gluCylinder(qobj, 20.0, 20.0, 4.0, 15, 1);
  //partialcylinder(10.0,20.0,0.0,360.0,4.0,slices);
  glEndList();
  /*tower blocks */
  glNewList(list+5, GL_COMPILE);
  glBindTexture(GL_TEXTURE_2D, texture);
     	partialcylinder(12.0,20.0,0.0,36.0,4.0,18);
  glEndList();
  
  glNewList(list+6, GL_COMPILE);
  glBindTexture(GL_TEXTURE_2D, towerTex);
     gluDisk(qobj, 0, 20.0, 15, 1);
  glEndList();
  gluDeleteQuadric(qobj);
}


void draw_castle(void) {
  int i;
  glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_CULL_FACE);
  GLfloat mat_ambient[] = {0.5, 0.5, 0.5, 1.0};
  GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_shininess[] = {20.0};
  GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};
  GLfloat model_ambient[] = {0.5, 0.5, 0.5, 1.0};
	glPushMatrix();
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

  
	
  glScalef(3, 3, 3);

  glShadeModel(GL_FLAT);
  glPushMatrix();	
 /* right castle wall */
  glTranslatef(55.5,-45.0,0.0);
  glCallList(list+1); 
  glPopMatrix();
 /* top of right castle wall */
  glPushMatrix();
  glTranslatef(70.5,25.0,0.0);
  for(i=0;i<18;i++) {
   if(!(i%2)) {
    glCallList(list+2);
   } else {
    glTranslatef(0.0,0.0,10.0);
   }
  }
  glPopMatrix();

  glPushMatrix();
 /* left  castle wall */
  glTranslatef(-55.5,-45.0,0.0);
  glCallList(list+1); 
  glPopMatrix();
 /* top of left castle wall */
  glPushMatrix();
  glTranslatef(-55.5,25.0,0.0);
  for(i=0;i<18;i++) {
   if(!(i%2)) {
    glCallList(list+2);
   } else {
    glTranslatef(0.0,0.0,10.0);
   }
  }
  glPopMatrix();


  glPushMatrix();
 /* back catle wall */
  glTranslatef(-35.0,-45.0,0.0);
  glRotatef(90.0,0.0,1.0,0.0);
  glCallList(list+1); 
  glPopMatrix();
 /* top of back castle wall */
  glPushMatrix();
  glTranslatef(-35.5,25.0,-15.0);
  glRotatef(90.0,0.0,1.0,0.0);
  for(i=0;i<18;i++) {
   if(!(i%2)) {
    glCallList(list+2);
   } else {
    glTranslatef(0.0,0.0,10.0);
   }
  }
  glPopMatrix();

	glPushMatrix();
	glTranslatef(-35.0, -45, 90);
	draw_frontWall(90, 20, 70);
	glPopMatrix();
	
	
  	//solidRectangle(90.0,20.0,70.0, 1.0, 1.0);
  
   
  
 /* glPushMatrix();
 // front castle wall 
  glTranslatef(-35.0, -45.0, 110.0);
  glRotatef(90.0, 0.0, 1.0, 0.0);
  glCallList(list+1); 
  glPopMatrix();
 // top of front castle wall 
  glPushMatrix();
  glTranslatef(-35.5,25.0,110.0);
  glRotatef(90.0,0.0,1.0,0.0);
  for(i=0;i<18;i++) {
   if(!(i%2)) {
    glCallList(list+2);
   } else {
    glTranslatef(0.0,0.0,10.0);
   }
  }
  glPopMatrix();
*/  glShadeModel(GL_SMOOTH);



	
  glPushMatrix();
 /* front right tower */
  glTranslatef(66.3,55.0,100.0);
  glRotatef(90.0, 1.0, 0.0, 0.0);
  glCallList(list); 
  glPopMatrix();
 /* top of right front tower */
  glPushMatrix();
  glTranslatef(66.3,62.0,100.0);
  //bottom of tower top
     glPushMatrix();
     glTranslatef(0.0, -7.0, 0.0);
     glRotatef(90.0, 1.0, 0.0, 0.0);
     glCallList(list+6);
     glPopMatrix();
  glRotatef(90.0, 1.0, 0.0, 0.0);
  glCallList(list+3);
  glTranslatef(0.0,0.0,-4.0);
  glCallList(list+4);
  glTranslatef(0.0,0.0,-4.0);
  for(i=0;i<10;i++) {
   if(!(i%2)) {
     glCallList(list+5);
   } else {
     glRotatef(36.0, 0.0, 0.0, 1.0);
   }
  }
  glPopMatrix();

  glPushMatrix();
 /* rear right tower */
  glTranslatef(66.3,55.0,-10.0);
  glRotatef(90.0, 1.0, 0.0, 0.0);
  glCallList(list); 
  glPopMatrix();
 /* top of right rear tower */
  glPushMatrix();
  glTranslatef(66.3,62.0,-10.0);
  //bottom of tower top
     glPushMatrix();
     glTranslatef(0.0, -7.0, 0.0);
     glRotatef(90.0, 1.0, 0.0, 0.0);
     glCallList(list+6);
     glPopMatrix();
  glRotatef(90.0, 1.0, 0.0, 0.0);
  glCallList(list+3);
  glTranslatef(0.0,0.0,-4.0);
  glCallList(list+4);
  glTranslatef(0.0,0.0,-4.0);
  for(i=0;i<10;i++) {
   if(!(i%2)) {
     glCallList(list+5);
   } else {
     glRotatef(36.0, 0.0, 0.0, 1.0);
   }
  }
  glPopMatrix();

  glPushMatrix();
 /*front left tower */
  glTranslatef(-45.0,55.0,100.0);
  glRotatef(90.0, 1.0, 0.0, 0.0);
  glCallList(list); 
  glPopMatrix();
 /* top of left front tower */
  glPushMatrix();
  glTranslatef(-45.0,62.0,100.0);
  
  //bottom of tower top
     glPushMatrix();
     glTranslatef(0.0, -7.0, 0.0);
     glRotatef(90.0, 1.0, 0.0, 0.0);
     glCallList(list+6);
     glPopMatrix();
  glRotatef(90.0, 1.0, 0.0, 0.0);

  glCallList(list+3);
  glTranslatef(0.0,0.0,-4.0);

  glCallList(list+4);
  glTranslatef(0.0,0.0,-4.0);
  for(i=0;i<10;i++) {
   if(!(i%2)) {
     glCallList(list+5);
   } else {
     glRotatef(36.0, 0.0, 0.0, 1.0);
   }
  }
  glPopMatrix();


  glPushMatrix();
 /* rear left tower */
  glTranslatef(-45.0,55.0,-10.0);
  glRotatef(90.0, 1.0, 0.0, 0.0);
  glCallList(list); 
  glPopMatrix();
 /* top of left rear tower */
  glPushMatrix();
  glTranslatef(-45.0,62.0,-10.0);
  //bottom of tower top
     glPushMatrix();
     glTranslatef(0.0, -7.0, 0.0);
     glRotatef(90.0, 1.0, 0.0, 0.0);
     glCallList(list+6);
     glPopMatrix();
  glRotatef(90.0, 1.0, 0.0, 0.0);
  glCallList(list+3);
  glTranslatef(0.0,0.0,-4.0);
  glCallList(list+4);
  glTranslatef(0.0,0.0,-4.0);
  for(i=0;i<10;i++) {
   if(!(i%2)) {
     glCallList(list+5);
   } else {
     glRotatef(36.0, 0.0, 0.0, 1.0);
   }
  }
  glPopMatrix();
 
  glPushMatrix();
  glTranslatef(12.0,0.0,90.0);
  
  //draw_bridge_frame();
  //draw_bridge();

  glPopMatrix();
  
  glPopMatrix();
    glDisable(GL_TEXTURE_2D);  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

}

void draw_bridge_frame(void) {

 /* top boarder arch */
   glDisable(GL_TEXTURE_2D);
  glPushMatrix();
  glRotatef(-90.0,0.0,0.0,1.0);
  partialcylinder(15.0,20.0,0.0,180.0,22.0,60);
  glPopMatrix();

 /* right boarder */
 
  glPushMatrix();

  glTranslatef(15.0,-40.0,0.0);
  solidRectangle(22.0,5.0,40.0, 1.0, 1.0);
  glPopMatrix();

 /* left boarder */
  glPushMatrix();
  glTranslatef(-20.0,-40.0,0.0);
  solidRectangle(22.0,5.0,40.0, 1.0, 1.0);
  glPopMatrix(); 
  glEnable(GL_TEXTURE_2D);
}

void draw_bridge(void) {

  GLfloat color_bridge[] = {0.38, 0.2, 0.0, 1.0};
	
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color_bridge);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color_bridge);

 /* top of drawbridge */
 glBindTexture(GL_TEXTURE_2D, drawbridgeTex);
/*  glPushMatrix();
  glTranslatef(0.0,0.0,1.0);
  glRotatef(-90.0,0.0,0.0,1.0);
  partialcylinder(0.0,15.0,0.0,180.0,20.0,60);
  glPopMatrix();

 /* drawbridge bottom */
/*  glPushMatrix();
  glTranslatef(-15.0,-40.0,1.0);
  solidRectangle(20.0,30.0,40.0, 5.0, 5.0);
  glPopMatrix();
*/
}



void draw_frontWall(float length, float width, float height) {
	
	float division = length / 3;
	
	glBegin(GL_QUADS);
	//front left
	glTexCoord2f(0.0, 0.0); glNormal3f(0, 0, -1); glVertex3f(0,0,0);
	glTexCoord2f(0.0, 1.0); glVertex3f(0,height,0);   
	glTexCoord2f(1.0/3.0, 1.0); glVertex3f(division,height,0);   
	glTexCoord2f(1.0/3.0, 0.0); glVertex3f(division,0,0);
	
	//back left
	glTexCoord2f(0.0, 0.0); glNormal3f(0, 0, 1); glVertex3f(0,0,width);   
	glTexCoord2f(0.0, 1.0); glVertex3f(0,height,width);   
	glTexCoord2f(1.0/3.0, 1.0); glVertex3f(division,height,width);   
	glTexCoord2f(1.0/3.0, 0.0); glVertex3f(division,0,width);
	
	//front mid
	glTexCoord2f(1.0/3.0, 2.0/3.0);  glNormal3f(0, 0, -1); glVertex3f(division,height*2.0/3.0,0);   
	glTexCoord2f(1.0/3.0, 1.0); glVertex3f(division,height,0);   
	glTexCoord2f(2.0/3.0, 1.0); glVertex3f(division*2,height,0);   
	glTexCoord2f(2.0/3.0, 2.0/3.0); glVertex3f(division*2,height*2.0/3.0,0);
		
	//back mid
	glTexCoord2f(1.0/3.0, 2.0/3.0); glNormal3f(0, 0, 1); glVertex3f(division,height*2.0/3.0,width);   
	glTexCoord2f(1.0/3.0, 1.0); glVertex3f(division,height,width);   
	glTexCoord2f(2.0/3.0, 1.0); glVertex3f(division*2,height,width);   
	glTexCoord2f(2.0/3.0, 2.0/3.0); glVertex3f(division*2,height*2.0/3.0,width);
	
	//front right
	glTexCoord2f(2.0/3.0, 0.0); glNormal3f(0, 0, -1); glVertex3f(division*2,0,0);   
	glTexCoord2f(2.0/3.0, 1.0); glVertex3f(division*2,height,0);   
	glTexCoord2f(1.0, 1.0); glVertex3f(length,height,0);   
	glTexCoord2f(1.0, 0.0); glVertex3f(length,0,0);
	
	//back right
	glTexCoord2f(2.0/3.0, 0.0); glNormal3f(0, 0, 1); glVertex3f(division*2,0,width);   
	glTexCoord2f(2.0/3.0, 1.0); glVertex3f(division*2,height,width);   
	glTexCoord2f(1.0, 1.0); glVertex3f(length,height,width);   
	glTexCoord2f(1.0, 0.0); glVertex3f(length,0,width);
	
	//inside left
	glTexCoord2f(1.0/3.0, 0.0); glNormal3f(1, 0, 0); glVertex3f(division, 0, 0);
	glTexCoord2f(1.0/3.0, 2.0/3.0); glVertex3f(division, height*2.0/3.0, 0.0);
	glTexCoord2f(2.0/3.0, 2.0/3.0); glVertex3f(division, height*2.0/3.0, width);
	glTexCoord2f(2.0/3.0, 0.0); glVertex3f(division, 0, width);
	
	//inside right
	glTexCoord2f(1.0/3.0, 0.0);  glNormal3f(-1, 0, 0); glVertex3f(division*2, 0, 0);
	glTexCoord2f(1.0/3.0, 2.0/3.0); glVertex3f(division*2, height*2.0/3.0, 0.0);
	glTexCoord2f(2.0/3.0, 2.0/3.0); glVertex3f(division*2, height*2.0/3.0, width);
	glTexCoord2f(2.0/3.0, 0.0); glVertex3f(division*2, 0, width);
	
	//inside top
	glTexCoord2f(1.0/3.0, 0.0); glNormal3f(0, -1, 0); glVertex3f(division, height*2.0/3.0, 0);
	glTexCoord2f(1.0/3.0, 1.0/3.0); glVertex3f(division, height*2.0/3.0, width);
	glTexCoord2f(2.0/3.0, 1.0/3.0); glVertex3f(division*2, height*2.0/3.0, width);
	glTexCoord2f(2.0/3.0, 0.0); glVertex3f(division*2, height*2.0/3.0, 0);
	
	//left outside
	glTexCoord2f(2.0/3.0, 0.0); glNormal3f(-1, 0, 0);glVertex3f(0,0,0);   
	glTexCoord2f(2.0/3.0, 1.0); glVertex3f(0,height,0);   
	glTexCoord2f(1.0, 1.0); glVertex3f(0,height,width);   
	glTexCoord2f(1.0, 0.0); glVertex3f(0,0,width);
	
	//left bottom
	glTexCoord2f(1.0/3.0, 0.0); glNormal3f(0, -1, 0); glVertex3f(0,0,0);   
	glTexCoord2f(1.0/3.0, 1.0/3.0); glVertex3f(0,0,width);   
	glTexCoord2f(1.0, 1.0/3.0); glVertex3f(division,0,width);   
	glTexCoord2f(1.0, 0.0); glVertex3f(division,0,0);
		
	//top
	glTexCoord2f(0.0, 1.0); glNormal3f(0, 1, 0); glVertex3f(0.0, height, 0.0);
	glTexCoord2f(0.0, 2.0/3.0); glVertex3f(0.0, height, width);
	glTexCoord2f(1.0, 2.0/3.0); glVertex3f(length, height, width);
	glTexCoord2f(1.0, 1.0); glVertex3f(length, height, 0.0);
	
	glEnd();
	
	glPushMatrix();
	glTranslatef(division, 0.0, width);
	
	glBindTexture(GL_TEXTURE_2D, drawbridgeTex);
	glBegin(GL_QUADS);
	glTexCoord2f(0.1, 0.0); glVertex3f(0.0, 0.0, 0.0);
	glTexCoord2f(0.1, 1.0); glVertex3f(0.0, 2.0/3.0*height, 0.0);
	glTexCoord2f(0.9, 1.0); glVertex3f(division, 2.0/3.0*height, 0.0);
	glTexCoord2f(0.9, 0.0); glVertex3f(division, 0.0, 0.0);
	
	glTexCoord2f(0.1, 0.0); glVertex3f(0.0, 0.0, 3.0);
	glTexCoord2f(0.1, 1.0); glVertex3f(0.0, 2.0/3.0*height, 3.0);
	glTexCoord2f(0.9, 1.0); glVertex3f(division, 2.0/3.0*height, 3.0);
	glTexCoord2f(0.9, 0.0); glVertex3f(division, 0.0, 3.0);
	
	glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(0.0, 2.0/3.0*height, 0.0);
	glTexCoord2f(0.1, 1.0); glVertex3f(0.0, 2.0/3.0*height, 3.0);
	glTexCoord2f(0.1, 0.0); glVertex3f(0.0, 0.0, 3.0);
	
	
	glTexCoord2f(1.0, 1.0); glVertex3f(division, 0.0, 0.0);
	glTexCoord2f(1.0, 3.0/4.0); glVertex3f(division, 2.0/3.0*height, 0.0);
	glTexCoord2f(3.0/4.0, 3.0/4.0); glVertex3f(division, 2.0/3.0*height, 3.0);
	glTexCoord2f(3.0/4.0, 1.0); glVertex3f(division, 0.0, 3.0);
	glEnd();
	glPopMatrix();
	
}

void ShutDown_castle() {
	glDeleteLists(list, 7);
	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &drawbridgeTex);

	
	
}
