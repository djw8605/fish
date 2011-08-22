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

void solidRectangle(float length, float width, float height, float texx, float texy) {


 /*top of rectangle */
  glBegin (GL_QUADS);
   glTexCoord2f(0.0, 0.0); glVertex3f(0,0,0);          //point #1
   glTexCoord2f(texx, 0.0); glVertex3f(0,0,length);     //point #2
   glTexCoord2f(texx, texy); glVertex3f(width,0,length); //point #3
   glTexCoord2f(0.0, texy); glVertex3f(width,0,0);      //point #4

 /*bottome of rectangle */
   glTexCoord2f(0.0, 0.0); glVertex3f(0,height,0);
   glTexCoord2f(texx, 0.0); glVertex3f(0,height,length);
   glTexCoord2f(texx, texy); glVertex3f(width,height,length);
   glTexCoord2f(0.0, texy); glVertex3f(width,height,0);

 /*left side of rectangle */
   glTexCoord2f(0.0, 0.0); glVertex3f(0,0,0);
   glTexCoord2f(texx, 0.0); glVertex3f(0,0,length);
   glTexCoord2f(texx, texy); glVertex3f(0,height,length);
   glTexCoord2f(0.0, texy); glVertex3f(0,height,0);

 /*right side of rectangle */
   glTexCoord2f(0.0, 0.0); glVertex3f(width,0,0);
   glTexCoord2f(texx, 0.0); glVertex3f(width,0,length);
   glTexCoord2f(texx, texy); glVertex3f(width,height,length);
   glTexCoord2f(0.0, texy); glVertex3f(width,height,0);

 /* rear end of rectangle */
   glTexCoord2f(0.0, 0.0); glVertex3f(0,0,0);
   glTexCoord2f(texx, 0.0); glVertex3f(0,height,0);
   glTexCoord2f(texx, texy); glVertex3f(width,height,0);
   glTexCoord2f(0.0, texy); glVertex3f(width,0,0);

 /* front end of rectangle */
   glTexCoord2f(0.0, 0.0); glVertex3f(0,0,length);
   glTexCoord2f(texx, 0.0); glVertex3f(0,height,length);
   glTexCoord2f(texx, texy); glVertex3f(width,height,length);
   glTexCoord2f(texx, texy); glVertex3f(width,0,length);
  glEnd();


}
