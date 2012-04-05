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





#ifndef COLLISIONMAP_H
#define COLLISIONMAP_H

#include "aquarium.h"

struct ThreeFloat {

	float x;
	float y;
	float z;

};



class collisionMap {

public:
	collisionMap();
	collisionMap(int size, Fish** fishPointer);
	~collisionMap();
	void setFishPointer(Fish** fishPointer);
	void resetSize(int newSize);
    int getClosestFish(int fishNum);
    int getClosestFishLeft(int fishNum);
    int getClosestFishDown(int fishNum);
    //float getSwarmDirection(int fishNum);
    float getSwarmYDirection(int fishNum);
    float getSwarmX(int fishNum);
    float getSwarmZ(int fishNum);
    void updateDistances();
    float getDistance(int fishIndex1, int fishIndex2);

    

private:
	//collisionPointer** BuildMap();
	int mapSize;
	Fish** fishPointer;
        short* closestFish;
        ThreeFloat* positions;
        float counter;
	float ycounter;
	char* clusters;
	//float dr2;
	float* dx;
	float* dz;
	float* ydir;
};



#endif
