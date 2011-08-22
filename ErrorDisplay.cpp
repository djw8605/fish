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


#include "ErrorDisplay.h"

#ifndef WIN32
#include <GL/glut.h>
#include "FreeType.h"
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <iostream>
#include <stdio.h>
#include <list>
#include <string>
#include <sstream>

#include "aquarium.h"

#define MAXSIZE 5
#define TEXTSPEED 25

#ifndef WIN32
extern freetype::font_data font;
#endif

using namespace std;

ErrorDisplay::ErrorDisplay() {
	nextID = 1;
	
}

ErrorDisplay::~ErrorDisplay() {
	
	
	
}


int ErrorDisplay::addMessage(const char* message) {
#ifndef WIN32

	ErrorMessage mess;
	mess.timeOut = 0;
	if(!message) {				//if the message is null, report nothing
		return 0;	
	}
	if((message[0] == '<') && (messages.size() < MAXSIZE)) {
		
		int endBracket = 0, i = 0;
		for(i = 0; i < strlen(message); i++) {	//find the ending bracket
			if(message[i] == '>') {
				endBracket = i;
				break;
			}

		} //end for		
	
//		printf("found time constraint..");
		char* time = new char[i];
		for(i = 1; i < endBracket; i++)
			time[i-1]=message[i];

		time[i] = '\0';
//		printf("set to %s...", time);
		mess.timeOut = atof(time)+timer;	// add to current time
//		printf("time out = %lf\n", mess.timeOut);
		delete time;
		stringstream sstream;		//create to easily transfer it to a string
		sstream << (message+endBracket+1);			//copy the message to the stream
		mess.mess = sstream.str();	//send the message to the message structure;
		mess.id = nextID++;			//set the id for the message
		messages.push_back(mess);
		return mess.id;
	}
	if(message[0] == '1') {
		
		stringstream sstream;		//create to easily transfer it to a string
	sstream << (message+1);			//copy the message to the stream
	mess.mess = sstream.str();	//send the message to the message structure;
	mess.id = nextID++;			//set the id for the message
	messages.push_front(mess);
		
		
		
	} else {
	
	if(messages.size() >= MAXSIZE)
		return 0;
	stringstream sstream;		//create to easily transfer it to a string
	sstream << message;			//copy the message to the stream
	mess.mess = sstream.str();	//send the message to the message structure;
	mess.id = nextID++;			//set the id for the message
	messages.push_back(mess);	//push the message onto the list
	}
	return mess.id;				//return the message id
#else
	return 0;
#endif
}


bool ErrorDisplay::removeMessage(int id) {
#ifndef WIN32
	
	if(id == 0)															//if 0, invalid, return false
		return false;
	
	bool found = false;
	list<ErrorMessage>::iterator iter;
	
	for(iter = messages.begin(); iter != messages.end(); iter++) {		//search for the message
		if(iter->id == id)	{											//if found
			messages.erase(iter);										//remove the message
			return true;												//return true, did find the message
		}
	}
	return false;														//couldn't find the element
	
	
	
#endif
	return false;
}


void ErrorDisplay::render() {
#ifndef	WIN32
	timer += getTime();
	if(!messages.size())		//if there's no messages, don't render, DUH!
		return;
	glPushMatrix();
	
	glColor4f(0.0, 0.0, 0.0, 1.0);
	int i = glutGet(GLUT_WINDOW_WIDTH);
	//printf("rendering\n");
	//calling glutGet could be more efficient if i didn't do it every time
	list<ErrorMessage>::iterator iter;
	int heightFactor = 0;
	for(iter = messages.begin(); iter != messages.end(); iter++, heightFactor++) {
		if((timer > iter->timeOut) && iter->timeOut) {
			//printf("removing message %s\n", iter->mess.c_str());
			removeMessage(iter->id);
			//this->render();
			glPopMatrix();
			return;
		}
		
		i = glutGet(GLUT_WINDOW_WIDTH) - iter->mess.size() * 15;
		freetype::print(font, i , glutGet(GLUT_WINDOW_HEIGHT) - 30 - (heightFactor*20), iter->mess.c_str());
	}

	glPopMatrix();
		
	
	
#endif
}



