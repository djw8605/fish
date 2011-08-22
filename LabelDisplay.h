/*
 * LabelDisplay.h
 *
 *  Created on: May 21, 2010
 *      Author: Derek Weitzel
 */

#ifndef LABELDISPLAY_H_
#define LABELDISPLAY_H_

#include <GL/glut.h>
#include <GL/glx.h>

#define _labels LabelDisplay::GetInstance()

class LabelDisplay {


public:
	LabelDisplay();
	~LabelDisplay();
	static LabelDisplay* GetInstance();
	void Render();
	void InitDisplayLists();

private:

	GLuint m_labelList;



};




#endif /* LABELDISPLAY_H_ */
