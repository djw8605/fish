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


*/

/*

Authors: Cory Lueninghoener, Derek Weitzel

*/

#ifndef AQUARIUM_H
#define AQUARIUM_H

#ifdef WIN32
#define	WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif

//used for callbacks for the resize of the fishies
class FishResize {
public:
	virtual void FishResizeCallback(int newNumFish){};
};



//used for theadable classes
class Runnable {
public:
	virtual void Start(){};
};


//used for stats
class NodeInfo {
	public:
	NodeInfo() {
		info = 0;

	}
	void init(int num) {
		info = new char*[num];
		int i = 0;
		for(i = 0; i < num; i++)
			info[i] = 0;
	}
	char** info;
};

struct Stats {
	char** names;
	char** print;
	char** order;

};

class ClusterInfo {
public:
	ClusterInfo() {
		connectionID = readID = 0;
	}
	char* name;			//obvious
	char* ip;			//again, obvious
	float* color;		//color that the fish will be
	int numNodes;		//number of nodes in the cluster
	int lowerFish;		//lower bound of fish that belong to this cluster (depreciated)
	int connectionID;	//for connection errors
	int readID;			//for read errors
	char* website;		//used for the graphs
	char* command;       //used if the cluster is of type OSG
	int port;			// Port number
};


class Fish
{
public:
	Fish() {

		cpus = 2;
		picked = false;
		deadnumberList = 0;
		alivenumberList = 0;
		targetLoad = 0;
		bubbleCounter = 0;
		colorCounter = 0;
		planeFish = 0;

	};
  double location[3];
  double homedepth;
  double rotation;
  int mouthloc;
  int direction;
  double finrotation;
  double oldfinrotation[3];
  double findirection;
  double speed;
  int alive;
  double ydirection;
  double dx;
  double dz;

  int colliding;

  int bobcounter;

  double size;
  int growing;
  int shrinking;
  double targetsize;

  int dying;
  int undying;
  int deathangle;

  int blinking;
  float blinkcount;

  //used for whatever on side
  char* print;

  //used for collision immunity
  double immune;

  //used for rotation
  double targetrotation;

  //used to slow rotation when fishies are far apart
  double rotationspeed;

  //used for calllists for numbers on side
  int idnumber[3];

  //used as a counter for the rotating number of the fish, when dead
  double deadcounter;

  //used to document the number of cpu's in the computer
  int cpus;

  //used to store the value of the load on the cpu's
  double load_one;

  //used as the name ganglia will call it
  char* fishname;

  //cluster this fish belongs to
  char* fishcluster;

  //used to determine if ganglia is reporting it
  bool checked;

  //used for picking
  int picking[3];

  //used to distinguish this fish
  bool picked;

  //used for stats
  NodeInfo nodeinfo;

  //pointers to fish on same or near planes
  Fish** planeFish;

  //used as the amount of fish on same plane
  int numplaneFish;

  //is the display list for the numbers on side
  int deadnumberList;

  //display list for alive number list
  int alivenumberList;

  //target size
  float targetLoad;

  //for exploding bubble
  double bubbleCounter;

  //so each fish knows what cluster it's a part of
  ClusterInfo* clusterPtr;

  //for colors when rotating colors
  float colorCounter;

};





#define EYE_OPEN 0
#define EYE_HALF_CLOSED 1
#define EYE_CLOSED 2
#define EYE_HALF_OPEN 3

#define MODEL_REFLECT_LEFT 0
#define MODEL_REFLECT_RIGHT 1
#define MODEL_REAL 2
#define MODEL_REFLECT_BACK 3

#define xmin -1333
#define xmax 1333
#define ymin -500
#define ymax 500
#define zmin -500
#define zmax 500

#define CASTLESELECTED 1



//Function Definitions
double getTime();
void ShutDown_lighthouse();
void draw_lighthouse();
void init_lighthouse();
void parse_config();
void begin_picking(int x, int y);
void display();
void init_fish();
void request_image(unsigned int* Tex, char* nodeName, Fish* fish);
void image_incriment();
//bool IsExtensionSupported(char* ext);

#ifdef WIN32
DWORD WINAPI RunThread(LPVOID ptr);
#endif

#endif //end AQUARIUM_H
