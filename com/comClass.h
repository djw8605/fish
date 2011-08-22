

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



#ifndef COMCLASS_H
#define COMCLASS_H

#ifndef WIN32
#include <pthread.h>
#else
#include <windows.h>
#endif
#include "xercesc/util/PlatformUtils.hpp"

#include "xercesc/dom/DOM.hpp"
#include "xercesc/dom/DOMImplementation.hpp"
#include "xercesc/dom/DOMImplementationLS.hpp"
//#include "xercesc/dom/DOMWriter.hpp"

#include "xercesc/framework/StdOutFormatTarget.hpp"
#include "xercesc/framework/LocalFileFormatTarget.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/util/XMLUni.hpp"

#include <stdio.h>
#include <iostream>
#include "../aquarium.h"

using namespace std;

XERCES_CPP_NAMESPACE_USE

class comClass {
	
public:
	comClass();
	~comClass();
	void init();
	void Start(bool NewThread);
	void possibleAddNode();
	
	


	
private:
	Fish* findFish(char* name, const ClusterInfo* cluster);
	Fish* addNode(char* name, ClusterInfo* cluster);
	int connectionErrorID;
	int receiveErrorID;
#ifndef WIN32
	pthread_t thread;
#else
	HANDLE thread;
#endif
	char* gangFile;
	XercesDOMParser *parser;
	int NewFishCount;
	Fish* NewFish;

	//thread proofing the adding nodes stuff
	bool handlingNewFish;
	bool handlingFish;
	
	//thread proof the running of the update
	#ifndef WIN32
	pthread_mutex_t updatingFish;
	#else
	HANDLE updatingFish;
	#endif	
	
	
};




#endif //COMCLASS_H
