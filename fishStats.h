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

#ifndef FISHSTATS_H
#define FISHSTATS_H



class fishStats {
	
	public:
	fishStats();
	~fishStats();
	void selectFish(int fishnum);
	void render();	
	
	
	protected:
	int fishnum;
	int oldfishnum;
	unsigned int graphTex;
	
	
	
};














#endif



