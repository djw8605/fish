

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

#define DEBUGLOCAL


#include "comClass.h"
#include "../aquarium.h"
#include "../ErrorDisplay.h"
#include "../collisionMap.h"
#ifndef WIN32
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/time.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
//#include <netinet/ip_var.h>
#include <netdb.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>

#else //WIN32
#define	WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <winsock.h>
#include <time.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fstream>
#include <sstream>
#include <math.h>


#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
//#include <xercesc/dom/DOMWriter.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

//#define USE_BUFFER

XERCES_CPP_NAMESPACE_USE
using namespace std;

extern int errno;
extern Fish* fish;
extern int FISHCOUNT;
bool WriteDone = false;
extern bool PrintFrames;
extern ErrorDisplay errorMess;
extern int numClusters;
extern ClusterInfo* clusters;
extern Stats stats;
extern int statFields;

Fish* newFish;
bool addNewFish;

extern collisionMap* cm;




//Fish* findFish(char* name, const ClusterInfo* cluster);
//void addNode(char* name, const ClusterInfo* cluster);

//seconds for a timeout before exiting xml parsing
#define RECEIVETIMEOUT 4
#ifndef WIN32
	void* test(void* ptr) {
#else
	DWORD WINAPI ThreadFunc(LPVOID ptr) {
#endif
#ifndef WIN32
	pthread_detach(pthread_self());
	reinterpret_cast<comClass*>(ptr)->init();
	pthread_exit(0);
	return NULL;
#else
	reinterpret_cast<comClass*>(ptr)->init();
	return NULL;
#endif
}

static int CheckReadable(int sock) {

	fd_set fdread;
	FD_ZERO(&fdread);
	FD_SET(sock, &fdread);
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	int socketsRetained = select(sock+1, &fdread, 0, 0, &timeout);
	FD_ZERO(&fdread);
	return socketsRetained;

}

comClass::comClass() {
#ifdef WIN32
	WSADATA WsaDat;
	WSAStartup(MAKEWORD(2,2), &WsaDat);
#endif
	//error = pthread_create(&thread, NULL, test, this);
	connectionErrorID = 0;
	receiveErrorID = 0;
	NewFishCount = FISHCOUNT;
	NewFish = 0;

	#ifndef WIN32
	if(pthread_mutex_init(&updatingFish, 0))
	{
		printf("Unable to lock updating fish mutex, check pthead setup");
	}

	#endif

	handlingFish = handlingNewFish = false;

	#ifndef USE_BUFFER
	const char* origFile = "/tmp/gangout";
	bool done = false;
	string filename;
	filename = origFile;
	stringstream counter;
	int fileCounter = 0;
	char* temp;
	FILE* gangoutFile;
	do {
		temp = new char[filename.length()+1];

		strcpy(temp, filename.c_str());
#ifndef WIN32
		printf("testing %s\n", temp);
#endif
		gangoutFile = fopen(temp, "r");

		//gangout file already exists
		if(gangoutFile) {
#ifndef WIN32
			//filename.erase(0, filename.length());
			filename = origFile;
			stringstream sin;
			sin<<fileCounter;
			filename.append(sin.str());

			fileCounter++;
			fclose(gangoutFile);
			delete temp;
#else
			DeleteFile(origFile);
			done = true;
			gangFile = temp;
#endif

		} else {
			//gangout file doesn't exist
			done = true;
			gangFile = temp;

		}

	}while(done == false);
	#endif


}

comClass::~comClass() {
	if(parser)
		delete parser;
#ifndef WIN32
	stringstream commands;
	commands<<"rm "<<gangFile;
	system(commands.str().c_str());
#endif

}

void comClass::Start(bool NewThread) {

	//start a new thread to read the data in

	if(NewThread) {
#ifndef WIN32
		pthread_create(&thread, NULL, test, this);
#else

		CreateThread(0, 0, ThreadFunc, this, 0, 0);
#endif
	} else {
		init();
	}

}

struct SocktoFile {
	int sock;
	iostream *file;
};
#ifndef WIN32
void* WriteXML(void * sockfile) {
	pthread_detach(pthread_self());
#else
DWORD WINAPI WriteXML(LPVOID sockfile) {
#endif

	SocktoFile* socks = (SocktoFile*)sockfile;
	char temp[1000];
	int received = 999;
	while(received > 0) {
		if(CheckReadable(socks->sock)) {
			received = recv(socks->sock, temp, 999, 0);
		} else
			continue;
		//printf(".");
		//fflush(stdout);
		temp[received] = '\0';

		*(socks->file) << temp;
		//printf("output = %i\n", (int)temp[received-1]);
#ifndef WIN32
		//usleep(1000);
#else
		Sleep(1000/1000);
#endif
	}
	WriteDone = true;

	return 0;
}



void comClass::init() {
	#ifndef WIN32
	if(pthread_mutex_trylock(&updatingFish)) {
		printf("Unable to lock updatingFish");
		return;
	}
	//pthread_mutex_lock(&updatingFish);
	#endif
	static bool first = true;
	if(first)
		NewFishCount = FISHCOUNT;
	PrintFrames = false;
	Fish* fishChange;  //used to find the fish to modify properties
	int count = 0;
	XMLPlatformUtils::Initialize();

	//multiple cluster support
	for(count = 0; count < numClusters; count++) {
	WriteDone = false;
	//pointless xerces stuff


	parser = new XercesDOMParser;

	parser->setValidationScheme(XercesDOMParser::Val_Auto);

    parser->setDoNamespaces(false);
    parser->setDoSchema(false);
    parser->setValidationSchemaFullChecking(false);
    parser->setCreateEntityReferenceNodes(false);

	//used to retrieve error codes from various parts
	int error = 0;

	// For the clusters that use a command to get the ganglia info
	if(clusters[count].command != 0)
	{
	    printf("detected a command cluster, using command\n");
	    stringstream command;
	    command << clusters[count].command;
	    command << " > " << gangFile;
	    printf("Executing: %s\n", command.str().c_str());
	    system(command.str().c_str());
	}
	else
	{

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	if (clusters[count].port)
		saddr.sin_port = htons(clusters[count].port);
	else
		saddr.sin_port = htons(8649);
	saddr.sin_addr.s_addr = inet_addr(clusters[count].ip);

/*	#ifdef REDCLUSTER
	saddr.sin_addr.s_addr = inet_addr("129.93.243.225");
	#endif
	#ifdef PRAIRIEFIRE
	saddr.sin_addr.s_addr = inet_addr("172.16.100.2");
	#endif
	#ifdef DEBUGLOCAL
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	#endif
*/
#ifndef WIN32
	printf("\nConnecting...");
#endif
	fflush(stdout);
	error = connect(sock, (sockaddr*)&saddr, sizeof(sockaddr_in));

	//if cannot connect
	if(error) {
#ifndef WIN32
		printf("Error Connecting Socket\n");
		printf("Socket error is = %s\n", strerror(errno));
#endif
		if(!clusters[count].connectionID) {			//if no error is already displayed
			string temp = "1*Error connecting to ";
			//temp.append("Error connecting to ");
			temp.append(clusters[count].name);

			clusters[count].connectionID = errorMess.addMessage(temp.c_str());
		}
		fflush(stdout);
#ifndef	WIN32
		close(sock);
#else
		closesocket(sock);
#endif
		delete parser;
		parser = NULL;
//		XMLPlatformUtils::Terminate();
		//PrintFrames = true;
		continue;

	}

	/* If there is an error already displayed, remove it */
	if(clusters[count].connectionID) {
		errorMess.removeMessage(clusters[count].connectionID);
		clusters[count].connectionID = 0;
	}
#ifndef WIN32
	printf("%s\n", strerror(errno));
#endif
	//printf("Done\n");
//	printf("sock error = %i\n", error);
//		closesocket(sock);
//	printf("errno = %i\n", errno);


	//open and fill a file with the ouput of ganglia
#ifndef WIN32
	#ifndef USE_BUFFER
	printf("Opening %s to write...", gangFile);
	fflush(stdout);
	#endif
#endif
	//rm command, needed after first time
#ifndef USE_BUFFER
	stringstream commands;
	commands<<"rm "<<gangFile;

#ifndef WIN32
	system(commands.str().c_str());
#endif

	//need to touch the file, for some reason, it doesnt create the file
	stringstream touchcommand;
	touchcommand<<"touch ";
	touchcommand<<gangFile;
	//printf("file = %s\n", touchcommand.str().c_str());
#ifndef WIN32
	system(touchcommand.str().c_str());
#endif
	//set the write file to gangFile
	fstream fsfile(gangFile);
		#endif
	//for the threading
	SocktoFile sockfile;
	stringstream sstream;

	#ifdef USE_BUFFER
	sockfile.file = &sstream;
	#else
	sockfile.file = &fsfile;
	#endif
	sockfile.sock = sock;

	//this thread is for timeout purposes, so that it doesn't block the execution of the program
#ifndef WIN32
	pthread_t writeThread;
	pthread_create(&writeThread, 0, WriteXML, &sockfile);
#else
	HANDLE writeThread;
	writeThread = CreateThread(0, 0, WriteXML, &sockfile, 0, 0);

#endif
	//note, this gives the thread 2 seconds to complete, then it gives up
	int c = 0, b = 0;
	#define TIMEOUTSLEEP 10000
	while(WriteDone == false ) {//&& c < RECEIVETIMEOUT*1000000/TIMEOUTSLEEP) {
		if(b == 10) {
			printf(".");
			fflush(stdout);
			b = 0;
		}
		b++;
#ifndef	WIN32
		usleep(TIMEOUTSLEEP);
#else
		Sleep(TIMEOUTSLEEP/1000);
#endif
		c++;
	}
	c = 0;
	//could not write to file for some reason
	if(!WriteDone) {
#ifndef WIN32
		pthread_cancel(writeThread);
#else
		TerminateThread(writeThread, 0);
#endif
#ifndef WIN32
		printf("Unable to Receive from socket, or Write to file\n");
#endif
		if(!clusters[count].readID )	{		//if no error is already displayed
			string temp;
			temp.append("1*Ganglia not responding at:  ");
			temp.append(clusters[count].name);

			clusters[count].readID = errorMess.addMessage(temp.c_str());
		}
		fflush(stdout);
		#ifndef USE_BUFFER
		fsfile.close();
		#endif
#ifndef WIN32
		close(sock);
#else
		closesocket(sock);
#endif
		delete parser;
		parser = NULL;
//		XMLPlatformUtils::Terminate();
		//PrintFrames = true;

		continue;
	}




	#ifndef USE_BUFFER
	fsfile.close();
	#endif

#ifndef WIN32
	error = close(sock);
#else
	error = closesocket(sock);
#endif
#ifndef WIN32
	printf("Done\n", error);
	}
        if(clusters[count].readID ) {
                errorMess.removeMessage(clusters[count].readID );
                clusters[count].readID  = 0;
        }
	//begin parsing the file
	#ifndef USE_BUFFER
	printf("Parsing XML... file = %s\n", gangFile);
	#endif
#endif

	//start unsafe code -- new on 4-6-07
	#ifdef USE_BUFFER
	const char* stuff = sstream.str().c_str();

	fflush(stdout);
	int xmlBuffsize = reinterpret_cast<stringstream*>(sockfile.file)->str().length();
	const char* xmlBuff = reinterpret_cast<stringstream*>(sockfile.file)->str().c_str();

	char* xmlBufId = "XML File";


	XMLCh* xstringxmlbufid = XMLString::transcode(xmlBufId);
	MemBufInputSource mbuf((XMLByte*)xmlBuff, xmlBuffsize-1, xstringxmlbufid, false);

	parser->parse(mbuf);
	delete xstringxmlbufid;
	//delete mbuf;

	#else
	parser->parse(gangFile);
	#endif


	//end unsafe code -- 4-6-07
	//xercesc_2_7::DOMDocument* doc = parser->getDocument();
	DOMDocument* doc = parser->getDocument();
//	printf("after doc\n");
//	fflush(stdout);
//	printf("%s\n", XMLString::transcode(doc->getNodeName()));
	XMLCh* host = XMLString::transcode("HOST");
	DOMNodeList* list = doc->getElementsByTagName(host);
	delete host;
//	printf("after list\n");
	int a = 0;
#ifndef WIN32
	printf("got %i nodes\n", list->getLength());
#endif
	for(a = 0; a < list->getLength(); a++) {
	DOMNode* node = list->item(a);
	DOMNamedNodeMap* map = node->getAttributes();
	if(!map) {
#ifndef WIN32
		printf("failed to get map\n");
		fflush(stdout);
#endif
	}

	XMLCh *t = XMLString::transcode("NAME");
	char *s = XMLString::transcode(map->getNamedItem(t)->getNodeValue());
	delete t;

//	delete map;

//	printf("name = %s\n", s);
//	fflush(stdout);
	fishChange = findFish(s, clusters+count);

	//new, unregistered fish

	if(!fishChange) {
		string temp;
		temp.append("<60>Added Node:  ");
		temp.append(s);
		temp.append("@");
		temp.append(clusters[count].name);
			errorMess.addMessage(temp.c_str());
		fishChange = addNode(s, clusters+count);

		//continue;
		//printf("added %s\n", fishChange->fishname);
		//fflush(stdout);

	}
	fishChange->checked = true;

	if(!strcmp(s, "head")) {



	} else {
	//this means its a normal node

//		printf("node = %i\n", fishnum);

	}

	//delete s;

	DOMNodeList* list = node->getChildNodes();
	if(!list) {
#ifndef WIN32
		printf("unable to get child nodes");
#endif
		continue;
	}

	//find the tag with load_one value
	int i = 0;
	//find all of the stats
	for(i = 0; i < list->getLength(); i++) {


		DOMNamedNodeMap* search = list->item(i)->getAttributes();
		//problem with line below

		if(!search) {
			continue;
		}

		XMLCh* ts = XMLString::transcode("NAME");
		DOMNode* tt = search->getNamedItem(ts);
		delete ts;
		XMLCh* tb = XMLString::transcode("VAL");
		DOMNode* tv = search->getNamedItem(tb);
		char* val = XMLString::transcode(tv->getNodeValue());
		delete tb;
#ifndef WIN32
		if(!node)
			printf("node could not be found\n");
#endif

		char* name = XMLString::transcode(tt->getNodeValue());

		//delete tt;
		int c = 0;
		bool usedVal = false;
			//printf("after something %s\n", val);
	fflush(stdout);
		for(c = 0; c < statFields; c++) {
			if(!strcmp(name, stats.names[c])) {
				if(fishChange->nodeinfo.info[c])
					delete fishChange->nodeinfo.info[c];
				fishChange->nodeinfo.info[c] = val;
				usedVal = true;

				//printf("found %s = %s\n", stats.names[c], fishChange->nodeinfo.info[c]);
			}

		}
		if(!strcmp(name, "load_one")) {


			//save the value of the load into the stucture
			fishChange->load_one = atof(val);
			//usedVal	= true;

		}
		if(!strcmp(name, "cpu_num")) {


			fishChange->cpus = atoi(val);
			//usedVal = true;
		}

		if(name)
			delete name;
		if(!usedVal)
			delete val;


	}

	delete s;

//	printf("value of cpu_num = %i, load = %lf, divided = %lf\n", fishChange->cpus, fishChange->load_one, fishChange->load_one/(double)fishChange->cpus);
//	fflush(stdout);

	if(fishChange->targetLoad) {
		fishChange->targetsize = fishChange->load_one/fishChange->targetLoad;
	} else {
	fishChange->targetsize = fishChange->load_one/(double)fishChange->cpus;
	}
	//easier on the processor
#ifndef WIN32
	//usleep(1000);
#else
	Sleep(1000/1000);
#endif
	}
#ifndef WIN32
	printf("Done\n");
#endif
	fflush(stdout);
	delete parser;
	parser = NULL;



	}
	XMLPlatformUtils::Terminate();
	PrintFrames = true;
	//delete list;





	  		  	int i = 0;
	//determine dead fish
	  for(i = 0; i < FISHCOUNT; i++) {
  	//fish not read in the xml
  	if(!fish[i].checked) {
  		fish[i].alive = 0;
      fish[i].finrotation = 40;
      fish[i].oldfinrotation[0] = -10.5;
      fish[i].oldfinrotation[1] = -10.75;
      fish[i].oldfinrotation[2] = -11.0;
      fish[i].blinking = EYE_CLOSED;
      fish[i].targetsize = 1;
  	}
  	else if(fish[i].alive == 0) {
  		fish[i].alive = 1;
	   	fish[i].finrotation = 0.0;
	fish[i].oldfinrotation[0] = 0.0;
	fish[i].oldfinrotation[1] = 0.0;
	fish[i].oldfinrotation[2] = 0.0;


  	}
 }
 first = false;
 	#ifndef WIN32
	pthread_mutex_unlock(&updatingFish);

	#endif

}

//For finding the fish in the fish array
Fish* comClass::findFish(char* name, const ClusterInfo* cluster) {

	if(!name)
		return NULL;

	//make sure that threading problems don't arise
	while(handlingFish || handlingNewFish)
#ifndef WIN32
		usleep(50);
#else
		Sleep(50);
#endif

	handlingFish = handlingNewFish = true;

	int i = 0; //cluster->lowerFish;
	int fishCount = FISHCOUNT;
	if(addNewFish)
		fishCount = NewFishCount;
	for(i = 0; i < fishCount; i++) {
		if(!addNewFish) {
		//			printf("comparing %s and %s, difference = %i\n", fish[i].fishname, name, strcmp(fish[i].fishname, name));
		if(!strcmp(fish[i].fishname, name) && cluster == fish[i].clusterPtr) {
			handlingFish = handlingNewFish = false;
			return fish+i;

		}

	}
	else {
	//				printf("comparing %s and %s, difference = %i\n", NewFish[i].fishname, name, strcmp(fish[i].fishname, name));
	//printf("fishCount = %i, i = %i\n", fishCount, i);
	fflush(stdout);
		if(!strcmp(NewFish[i].fishname, name) && cluster == NewFish[i].clusterPtr) {
			handlingFish = handlingNewFish = false;
			return NewFish+i;
	}



	}
}
	//printf("counldn't find: %s\n", name);

	handlingFish = handlingNewFish = false;


	return NULL;







}


Fish* comClass::addNode(char* name, ClusterInfo* cluster){

	//make sure not to touch fish while render(main) loop is
	//adding the fish to the fish array
	while(handlingFish || handlingNewFish)
#ifndef WIN32
		usleep(50);
#else
		Sleep(50);
#endif

	handlingNewFish = true;
	handlingFish = true;

	Fish* temp;

	if(!NewFish) {
	//haven't added new fish this iteration
		NewFish = new Fish[NewFishCount+1];

	//copy info from fish
		for(int i = 0; i < NewFishCount; i++) {
		//printf("after whatever\n");
		//fflush(stdout);
			NewFish[i] = fish[i];

		}
	} else {
	//have added new fish

		temp = NewFish;
		NewFish = new Fish[NewFishCount+1];
		for(int a = 0; a < NewFishCount; a++) {
			NewFish[a] = temp[a];

		}
		delete temp;

	}

	int i;

	i = NewFishCount;
	char* newName = new char[strlen(name)+1];
	strcpy(newName, name);
	NewFish[i].fishname = newName;
	NewFish[i].clusterPtr = cluster;
	NewFish[i].fishcluster = cluster->name;
//	printf("new fish = %s\n", name);
	char* fishPrint = new char[3];
	fishPrint[0] = name[strlen(name)-3];
	fishPrint[1] = name[strlen(name)-2];
	fishPrint[2] = name[strlen(name)-1];

	NewFish[i].print = fishPrint;

	//NewFish[i].print = "tst";
	//printf("after memory allocation\n");
	//this is all copied from the main function
	NewFish[i].location[0] = (rand() % (int)(1.8*xmax))-xmax;
    NewFish[i].location[1] = (rand() % (int)(1.8*ymax))-(0.9*ymax) + 30;
    NewFish[i].location[2] = (rand() % (int)(1.8*zmax))-zmax;
    NewFish[i].homedepth = NewFish[i].location[1];
    NewFish[i].speed = 1;
    NewFish[i].rotation = rand() % 360;
    NewFish[i].finrotation = (rand() % 6)-3;
    NewFish[i].size = ((float)(rand()%100))/100.0;
    NewFish[i].targetsize = NewFish[i].size;
    NewFish[i].alive = 1;
    NewFish[i].dying = 0;
    NewFish[i].undying = 0;
    NewFish[i].deathangle = 0;
    NewFish[i].growing = 0;
    NewFish[i].shrinking = 0;
    NewFish[i].blinking = EYE_OPEN;
    NewFish[i].immune = 0;
    NewFish[i].targetrotation = NewFish[i].rotation;
    NewFish[i].rotationspeed = 1;
    NewFish[i].idnumber[0] = (int)fmod((double)i,10);
    NewFish[i].idnumber[1] = (int)(fmod((double)i,100)/10);
	NewFish[i].nodeinfo.init(statFields);
    if(i > 99)
  	 	NewFish[i].idnumber[2] = 1;
  	 else
  	 	NewFish[i].idnumber[2] = 0;
  	 NewFish[i].deadcounter = 0;
	NewFish[i].planeFish = 0;
    NewFish[i].picked = false;

    NewFish[i].bobcounter = rand() % 360;

    //fish[i].finrotation = 0;
    NewFish[i].oldfinrotation[0] = 0;
    NewFish[i].oldfinrotation[1] = 0;
    NewFish[i].oldfinrotation[2] = 0;

    if(rand()%2)
      NewFish[i].findirection = 1;
    else
      NewFish[i].findirection = -1;

    if(rand()%2)
      NewFish[i].direction = 1;
    else
      NewFish[i].direction = -1;





  	NewFishCount++;
  	//assign the fish array
  	addNewFish = true;
	//addingNewFish = false;
	handlingFish = handlingNewFish = false;
	return NewFish+i;

}


void comClass::possibleAddNode() {

	//instead of hanging this thread, just return so that
	//the rendering continues;
	if(handlingFish || handlingNewFish)
		return;

	if(addNewFish) {
		//set so that problems can't arise
		handlingFish = true;
		handlingNewFish = true;


		FISHCOUNT=NewFishCount;

		delete fish;
		fish = NewFish;
		NewFish = 0;	//reset newfish
		int i = 0;


		for(i = 0; i < NewFishCount; i++) {
			if(fish[i].planeFish)
				delete fish[i].planeFish;
		}
		cm->resetSize(FISHCOUNT);

	//no need for this planeFish code with the new collisionMap
/*	int a = 0, count = 0;
  for(i = 0; i < FISHCOUNT; i++) {
  	count = 0;
  	for(a = 0; a < FISHCOUNT; a++) {
  	  if(fabs(fish[i].location[1]-fish[a].location[1])<70 && i != a) {

  	  	count++;
  	  }

  	}
  	fish[i].planeFish = new Fish*[count];
  	fish[i].numplaneFish = count;
  	count = 0;

  	for(a = 0; a < FISHCOUNT; a++) {
	//printf("after count i = %i, a = %i\n", i, a);
	  fflush(stdout);
  	  if(fabs(fish[i].location[1]-fish[a].location[1])<70 && i != a) {
  	  	fish[i].planeFish[count] = fish+a;
  	  	count++;

  	  }
  	}
  	//printf("count = %i\n", count);

  }
		*/


	}


	addNewFish = false;
	//addingFish = false;
	handlingFish = handlingNewFish = false;

}
