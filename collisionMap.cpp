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




#include "collisionMap.h"
#include "aquarium.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>



#define TIMEUPDATE 5

//convenience functions (i'm lazy)

static inline int max(int a, int b) {
	if(a > b)
	   return a;
	return b;
}

static inline int min(int a, int b) {
	if(a > b)
	   return b;
	return a;
}


collisionMap::collisionMap() {

	mapSize = 0;
	fishPointer = 0;
        closestFish = 0;
        positions = 0;
        counter = 0;
	ydir = 0;
	dx = 0;
	dz = 0;
	clusters = 0;
	ycounter = 0;
}




collisionMap::collisionMap(int size, Fish** fishPointer) {

	mapSize = size;
        counter = 0;
	ycounter = 0;
        this->fishPointer = fishPointer;
	resetSize(mapSize);

}



void collisionMap::resetSize(int newSize) {
		
        closestFish = new short[newSize];
        positions = new ThreeFloat[newSize];
	clusters = new char[newSize];
	dx = new float[newSize];
	dz = new float[newSize];
	ydir = new float[newSize];
        counter = 0;
	ycounter = 0;
        mapSize = newSize;
	updateDistances();
	
	
}


collisionMap::~collisionMap() {

        delete closestFish;
        delete positions;
	delete clusters;
	delete dx;
	delete dz;
	delete ydir;

}


int collisionMap::getClosestFish(int fishNum) {

        if (counter == 0)
            updateDistances();
	
	//check if the fishNum is larger than the map, which makes it invalid
	if( (fishNum >= mapSize) || (fishNum < 0) )
	   return -1;

        return closestFish[fishNum];
	
}



float collisionMap::getSwarmYDirection(int fishNum) {

        if (counter == 0)
            updateDistances();
	
	//check if the fishNum is larger than the map, which makes it invalid
	if( (fishNum >= mapSize) || (fishNum < 0) )
	   return -1;

        return ydir[fishNum];
	
}

float collisionMap::getSwarmX(int fishNum) {

        if (counter == 0)
            updateDistances();
	
	//check if the fishNum is larger than the map, which makes it invalid
	if( (fishNum >= mapSize) || (fishNum < 0) )
	   return -1;

        return dx[fishNum];
}	

float collisionMap::getSwarmZ(int fishNum) {

        if (counter == 0)
            updateDistances();
	
	//check if the fishNum is larger than the map, which makes it invalid
	if( (fishNum >= mapSize) || (fishNum < 0) )
	   return -1;

        return dz[fishNum];
}
//this function updates the counters on every fish in the map, to make sure
//they update their locations
void collisionMap::updateDistances() {
     
     // Don't re-calculate too often.
     
     if (--counter > 0)
         return;
     counter = TIMEUPDATE+1;

     float outerPosX, outerPosZ, outerPosY, distX, distZ, dist, minDist, dr, y, x, z;
     short minPos, s;
     //int arrayPos = 0;
     for (int i = 0; i < mapSize; i++) {
         positions[i].x = (*fishPointer)[i].location[0];
         positions[i].y = (*fishPointer)[i].location[1];
         positions[i].z = (*fishPointer)[i].location[2];
	 clusters[i] = *(*fishPointer)[i].fishcluster;
     }
     for (int i = 0; i < mapSize; i++) {
         outerPosX = positions[i].x;
         outerPosY = positions[i].y;
         outerPosZ = positions[i].z;
         minPos = 0;
         minDist = 1e16;
	 s = 1;
	 x = outerPosX;
	 z = outerPosZ;
	 y = outerPosY;
         for (int j = 0; j < mapSize; j++) {
              if ((i == j) || (abs(positions[j].y - outerPosY) > 20))
                  continue;
              distX = positions[j].x-outerPosX;
              distZ = positions[j].z-outerPosZ;
              dist = distX*distX + distZ*distZ;
              if (dist < minDist) {
                  minPos = j;
                  minDist = dist;
              }
		//find all fish of identical cluster within a radius and get average positions
              if (dist < 1e5) {	
		if (clusters[i] == clusters[j]) {
		s++;
		x += positions[j].x;
		z += positions[j].z;
		y += positions[j].y;
		}
	      }
         }
         closestFish[i] = minPos;
	 dx[i] = x/s;
	 dz[i] = z/s;
	 ydir[i] = y/s;
     }

}

float collisionMap::getDistance(int fishIndex1, int fishIndex2) {

	float distX, distZ;
        distX = positions[fishIndex1].x-positions[fishIndex2].x;
        distZ = positions[fishIndex1].z-positions[fishIndex2].z;
	return sqrt(distX*distX + distZ*distZ);

}
