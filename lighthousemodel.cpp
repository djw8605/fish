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

#ifndef WIN32
#include <GL/glut.h>
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <math.h>
#include "aquarium.h"
#include "CTargaImage.h"
#include "shaders.h"


#pragma warning( disable: 4305 )

GLUquadricObj *qobj;
extern double degrad;
GLuint lightList;
GLuint lightTex;

//rotation speed of light
#define LIGHTSPEED 7.5

void init_lighthouse()
{
  qobj = gluNewQuadric();
  CTargaImage image2;
  if(image2.Load("./lighthouse.tga")) {

  glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &lightTex);
	
	glBindTexture(GL_TEXTURE_2D, lightTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);



	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image2.GetWidth(), image2.GetHeight(),
					GL_RGB, GL_UNSIGNED_BYTE, image2.GetImage());
			
  image2.Release();
  }
  
  
}

void draw_lighthouse()
{
  static float lightangle = 0;
  static int listMade = 0;

  GLfloat spotpos[] = { 0, 0, 0, 1 };
  GLfloat spotdir[] = { 1, 0, 0 };

  GLfloat glassamb[] = { 1, 1, 1, .2 };
  GLfloat glassdif[] = { 1, 1, 1, .4 };
  GLfloat glassspec[] = { 1, 1, 1, .8 };

  GLfloat bodyamb[] = { .6, .6, .6, 1 };
  GLfloat bodydif[] = { .6, .6, .6, 1 };
  GLfloat bodyspec[] = { .1, .1, .1, 1 };

  GLfloat lightamb[] = { .8, .8, 0, 1 };
  GLfloat lightdif[] = { .8, .8, 0, 1 };
  GLfloat lightspec[] = { .1, .1, .1, 1 };

  GLfloat lensamb[] = { 1, 1, 1, .7 };
  GLfloat lensdif[] = { 1, 1, 1, .7 };
  GLfloat lensspec[] = { 1, 1, 1, 1 };
	if(!listMade) {
		lightList = glGenLists(1);
		glNewList(lightList, GL_COMPILE);

  glPushMatrix();
  glRotated(90, -1, 0, 0);

  /* Body */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, bodyamb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, bodydif);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, bodyspec);
	gluQuadricTexture(qobj, true);

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D, lightTex);
  glPushMatrix();
  glTranslatef(0, 0, -60);
  gluCylinder(qobj, 20, 15, 80, 20, 1);
  glPopMatrix();
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glDisable(GL_TEXTURE_2D);
 

#ifndef WIN32
  glPushMatrix();
  glTranslatef(0, 0, 20);
  glutSolidTorus(2, 16, 10, 10);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0, 0, 40);
  glutSolidTorus(2, 16, 10, 10);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0, 0, -60);
  glutSolidTorus(2, 22, 10, 10);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0, 0, 40);
  glutSolidCone(15, 10, 20, 10);
  glPopMatrix();
  
  glPushMatrix();
  glTranslatef(0, 0, 50);
  glutSolidSphere(3, 10, 10);
  glPopMatrix();
  
  glPushMatrix();
  glTranslatef(0, 0, 20);
  gluCylinder(qobj, 2, 2, 10, 20, 1);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0, 0, 30);
  glutSolidSphere(2, 10, 10);
  glPopMatrix();

#endif

	//call list making lighthouse
	
  /* Light */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, lightamb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, lightdif);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, lightspec);
    glEndList();
  
  //don't compile again, list alread made
  listMade = 1;
	}
	
	
glCallList(lightList);
  //spotdir[0] = cos(lightangle*degrad);
  //spotdir[1] = sin(lightangle*degrad);

  glPushMatrix();
  {
    glTranslatef(0, 0, 30);
    glRotated(lightangle, 0, 0, 1);

    glPushMatrix();
    {
      glRotated(90, 0, 1, 0);
      gluCylinder(qobj, 6, 6, 4, 20, 1);
      gluDisk(qobj, 0, 6, 20, 10);
    }
    glPopMatrix();
    glLightfv(GL_LIGHT1, GL_POSITION, spotpos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotdir);
    
    glPushMatrix();
    {
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, lensamb);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, lensdif);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, lensspec);
      glTranslatef(4, 0, 0);
      glScalef(.3, 1, 1);
      glFrontFace(GL_CW);
#ifndef WIN32
      glutSolidSphere(6, 10, 10);
#endif
      glFrontFace(GL_CCW);
    }
    glPopMatrix();
  }
  glPopMatrix();
  
  
  lightangle += LIGHTSPEED * getTime();
#ifndef	WIN32
  lightangle = fmod(lightangle, 360.0);
#else
  lightangle = fmod(lightangle, 360.0f);
#endif

  /* Glass */
  glFrontFace(GL_CW);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glassamb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glassdif);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glassspec);
  
  glPushMatrix();  
  glTranslatef(0, 0, 20);
  gluCylinder(qobj, 15, 15, 20, 20, 1);
  glPopMatrix();
  glFrontFace(GL_CCW);
  glPopMatrix();
	
	

  
}

void ShutDown_lighthouse() {
	gluDeleteQuadric(qobj);
	glDeleteLists(lightList, 1);
	glDeleteTextures(1, &lightTex);
	
}
