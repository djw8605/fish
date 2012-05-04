/*
 * LabelDisplay.cpp
 *
 *  Created on: May 21, 2010
 *      Author: Derek Weitzel
 */

#include <GL/glut.h>

#include <GL/glx.h>


#include "LabelDisplay.h"
#include "FreeType.h"
#include "aquarium.h"

extern freetype::font_data font;
extern int numClusters;
extern ClusterInfo* clusters;

LabelDisplay::LabelDisplay()
{
	// Create the display list for labels
	this->InitDisplayLists();

}

LabelDisplay::~LabelDisplay()
{


}

/*
 *
 * Label Display is a singleton
 * Accessed with _labels
 *
 */
LabelDisplay* LabelDisplay::GetInstance()
{

	static LabelDisplay* ld = new LabelDisplay();
	return ld;
}


void LabelDisplay::Render()
{

	glCallList(this->m_labelList);

}

void LabelDisplay::InitDisplayLists()
{
	int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	this->m_labelList = glGenLists(1);
	glNewList(this->m_labelList, GL_COMPILE);
	glMatrixMode(GL_PROJECTION);
			    glPushMatrix();
			    glLoadIdentity();
			    GLint   viewport[4];
			    glGetIntegerv(GL_VIEWPORT, viewport);

			            gluOrtho2D(0.0, windowWidth,0.0,windowHeight);
			    glMatrixMode(GL_MODELVIEW);
			    glPushMatrix();
			    glLoadIdentity();


	glColor4f(0.0, 1.0, 0.0, 1.0);
	glTranslatef(0.0, 0.0, 0.0);
	int spaceBetween = 25;
	glDisable(GL_LIGHTING);
	for(int i = 0; i < numClusters; i++)
	{
		// Give the first 4 colors, the ambient colors
		glColor4f(clusters[i].color[0], clusters[i].color[1], clusters[i].color[2], 1.0);
		glBegin(GL_QUADS);
		glVertex3f(5.0, (windowHeight-5.0)-(i*spaceBetween), 0.0);
		glVertex3f(25.0, (windowHeight-5.0)-(i*spaceBetween), 0.0);
		glVertex3f(25.0, (windowHeight-25.0)-(i*spaceBetween), 0.0);
		glVertex3f(5.0, (windowHeight-25.0)-(i*spaceBetween), 0.0);
		glEnd();
		glColor4f(0.0, 0.0, 0.0, 1.0);
		freetype::print(font, 30, (windowHeight-23.0)-(i*spaceBetween), clusters[i].name);

	}

        // And now the text on the bottom.  
        glTranslatef(0.0, 0.0, 0.0);
        glColor4f(0.0, 0.0, 0.0, 1.0);
        freetype::print(font, 30, 5, "Health of the HCC Clusters");
        // Done with bottom text
	
        glEnable(GL_LIGHTING);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEndList();

}

