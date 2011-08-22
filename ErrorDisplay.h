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

#ifndef ERRORDISPLAY_H
#define ERRORDISPLAY_H

#include <list>
#include <string>

using namespace std;

struct ErrorMessage {
	string mess;
	int id;
	double timeOut;
};


class ErrorDisplay {
	
	public:
		ErrorDisplay();
		~ErrorDisplay();
		//function returns the unique id to the user
		int addMessage(const char* message);
		//function removes a message from the list
		bool removeMessage(int id);
		
		//function to render the messages
		void render();
	
	protected:
	list<ErrorMessage> messages;		//list of the message to be displayed
	int nextID;							//next valid, unused id number
	double timer;
	
	

	
	
	
};






#endif
