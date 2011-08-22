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
#include <stdlib.h>
#include <math.h>


#define PI 3.141592653589

void partialcylinder(float inner, float outer, float start, float finish, float height, float slices) {
  float degree, angle, rad;
  float p1,p2,p3,p4;
  float i;
  
  
  /*degrees to radians PI/180 */
  if(start > finish){
    degree = start - finish;
  }
  else{
    degree = finish - start;
  }
  
  if(slices == 0){
    angle = degree;
  }else {
    angle = degree/slices;
  }
  rad = angle * (PI /180.0);
  
  p1 = inner*sin(rad); p2 = inner*cos(rad);
  p3 = outer*sin(rad); p4 = outer*cos(rad);
  
  for(i=0.0; i<=degree; i+=angle) {
    if(inner <= 0.0 ) {
      
      /*block top */
      glBegin (GL_TRIANGLES);
      glVertex3f(0,0,0);
      glVertex3f(p3,p4,0);
      glVertex3f(-p3,p4,0);
      
      /*block bottom */
      glVertex3f(0,0,height);
      glVertex3f(p3,p4,height);
      glVertex3f(-p3,p4,height);
      glEnd();
      
      /*outer surface */
      glBegin (GL_QUADS);
      glVertex3f(p3,p4,0);
      glVertex3f(-p3,p4,0);
      glVertex3f(-p3,p4,height);
      glVertex3f(p3,p4,height);
      glEnd(); 
      
  } else {
    
    /*block end */	
    glBegin (GL_QUADS);
    glVertex3f(p1,p2,0);  //point #1
    glVertex3f(p3,p4,0);  //point #2
    glVertex3f(-p3,p4,0); //point #3
    glVertex3f(-p1,p2,0); //point #4
    
    /*block end */
    glVertex3f(p1,p2,height);
    glVertex3f(p3,p4,height);
    glVertex3f(-p3,p4,height);
    glVertex3f(-p1,p2,height);
    
    /*outer surface */
    glVertex3f(p3,p4,0);
    glVertex3f(-p3,p4,0);
    glVertex3f(-p3,p4,height);
    glVertex3f(p3,p4,height);
    
    /*inner surface */
    glVertex3f(p1,p2,0);
    glVertex3f(-p1,p2,0);
    glVertex3f(-p1,p2,height);
    glVertex3f(p1,p2,height);
    glEnd();
    
  }
    
    if(i<=0) {
      /*end walls */
      glBegin (GL_QUADS);
      glVertex3f(p1,p2,0);
      glVertex3f(p3,p4,0);
      glVertex3f(p3,p4,height);
      glVertex3f(p1,p2,height);
      glEnd();
    } else if(i>=degree) {
      glBegin (GL_QUADS);
      glVertex3f(-p1,p2,0);
      glVertex3f(-p3,p4,0);
      glVertex3f(-p3,p4,height);
      glVertex3f(-p1,p2,height);
      glEnd();
    }
    
    glRotatef(angle,0.0,0.0,1.0);
  }
}
