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


Authors: Cory Lueninghoener, Derek Weitzel

*/
#ifndef WIN32
#include <GL/glut.h>
#include <GL/glx.h>
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <math.h>
#include "aquarium.h"
#include <stdio.h>
#include <stdlib.h>
#include "FreeType.h"
#include "shaders.h"
//using namespace std;

extern Fish* fish;
extern double degrad;
extern int FISHCOUNT;

#ifndef WIN32
extern freetype::font_data font;
#endif

#pragma warning( disable: 4244 ) //double to float warning
#pragma warning( disable: 4305 )

//max size the fish will reach
#define FISHMAXSIZE 1.3

//size at which the fish begin to turn red
#define STARTRED 1.2
#define STARTCOLOR STARTRED
//size at which the numbers begin to show on sides
#define STARTNUMBER .8
//time for 1 rotation of the dead number display
#define ROTATIONTIME 4
//multiply with number of CPU's to extend, lengthen the fish
#define CPUSTRETCH 0
//used as the speed of the rotating color
#define COLORROTATESPEED 2
//used to change the size of the fish
#define FISHSCALAR (500.0/((float)FISHCOUNT+300))

#define PI 3.14159265

  GLfloat bodyamb[] = { 1, .6, 0, 1.0 };
  GLfloat bodydif[] = { 1, .6, 0, 1.0 };
  GLfloat bodyspec[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat emissive[] = { 0.0, 0.0, 0.0, 1.0 };

    GLfloat fbodyamb[] = { 1, .6, 0, 1.0 };
  GLfloat fbodydif[] = { 1, .6, 0, 1.0 };
  GLfloat fbodyspec[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat femissive[] = { 0.0, 0.0, 0.0, 1.0 };

#define	checkImageWidth 64
#define	checkImageHeight 64
extern GLubyte checkImage[checkImageHeight][checkImageWidth][3];
extern bool picking;

GLuint eyeLists;
GLuint fishList;
GLuint numberList;
GLuint tailLists;

#ifndef WIN32
PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;					// VBO Name Generation Procedure
PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;					// VBO Bind Procedure
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;					// VBO Data Loading Procedure
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;				// VBO Deletion Procedure
#endif

GLuint VBOVertices;
GLuint VBONormals;
GLuint VBONormalsLeft;
bool VBOSupported;
bool VBOUsed;

float fmin(float a, float b) {
	if(a <= b)
		return a;
	else
		return b;

}




void find_normal(double *a, double *b, double *c, double *w)
{
  int x=0, y=1, z=2;
  double u[3], v[3];

  u[x] = a[x]-b[x];
  u[y] = a[y]-b[y];
  u[z] = a[z]-b[z];

  v[x] = b[x]-c[x];
  v[y] = b[y]-c[y];
  v[z] = b[z]-c[z];

  w[x] = (u[y]*v[z] - u[z]*v[y]);
  w[y] = (u[z]*v[x] - u[x]*v[z]);
  w[z] = (u[x]*v[y] - u[y]*v[x]);

}

//void draw_fish(int fishnum)
//{
  /*
  GLfloat bodyamb[] = { 1, .6, 0, 1 };
  GLfloat bodydif[] = { 1, .6, 0, 1 };
  GLfloat bodyspec[] = { 0.0, 0.0, 0.0, 1 };
  */



  static GLfloat eyeamb[] = { 1, 1, 1, 1.0 };
  static GLfloat eyedif[] = { 1, 1, 1, 1.0 };
  static GLfloat eyespec[] = { 0, 0, 0, 1.0 };

  static GLfloat pupilamb[] = { 0, 0, 0, 1.0 };
  static GLfloat pupildif[] = { 0, 0, 0, 1.0 };
  static GLfloat pupilspec[] = { 0, 0, 0, 1.0 };

  GLfloat shine = 0;

  static int normscalculated = 0;

  static double A[3] = {  32.0,  -4.0,  0.0};
  static double B[3] = {  24.0,  16.0,  0.0};
  static double C[3] = {   8.0,  20.0,  0.0};
  static double D[3] = {  -6.0,  16.0,  0.0};
  static double E[3] = { -14.0,   4.0,  0.0};
  static double F[3] = { -22.0,   8.0,  0.0};
  static double G[3] = { -30.0,  21.0,  0.0};
  static double H[3] = { -34.0,  11.0,  0.0};
  static double I[3] = { -30.0,   0.0,  0.0};
  static double J[3] = { -34.0, -11.0,  0.0};
  static double K[3] = { -31.0, -21.0,  0.0};
  static double L[3] = { -22.0,  -8.0,  0.0};
  static double M[3] = { -14.0,  -4.0,  0.0};
  static double N[3] = { -10.0, -12.0,  0.0};
  static double O[3] = {   4.0, -22.0,  0.0};
  static double P[3] = {  24.0, -16.0,  0.0};
  static double Q[3] = {  30.0, -12.0,  0.0};
  static double R[3] = {  34.0,  -2.0,  0.0};
  static double S[3] = {  10.0,   6.0,  8.0};
  static double T[3] = {   9.0,  -6.0, 10.0};
  static double U[3] = { -14.0,   0.0,  4.0};

  static double eyeA[3] = { 16.0, 13.0, 1.0 };
  static double eyeB[3] = { 19.5, 14.5, 2.0 };
  static double eyeC[3] = { 23.0, 13.5, 2.0 };
  static double eyeD[3] = { 26.5,  8.5, 2.0 };
  static double eyeE[3] = { 27.0,  1.0, 2.0 };
  static double eyeF[3] = { 22.5, -2.0, 2.0 };
  static double eyeG[3] = { 17.0,  0.0, 2.0 };
  static double eyeH[3] = { 15.0,  7.0, 2.0 };
  static double eyeI[3] = { 23.0,  4.0, 5.0 };

  static double deadeyeA[3] = { 16.5, 13.0, 3 };
  static double deadeyeB[3] = { 24.5, 13.0, 3 };
  static double deadeyeC[3] = { 28.0, 10.0, 3 };
  static double deadeyeD[3] = { 28.0,  2.0, 3 };
  static double deadeyeE[3] = { 24.5, -1.0, 3 };
  static double deadeyeF[3] = { 16.5, -1.0, 3 };
  static double deadeyeG[3] = { 14.0,  2.0, 3 };
  static double deadeyeH[3] = { 14.0, 10.0, 3 };

  double labelA[3] = { -11, -22, 0 };
  double labelB[3] = { -11, -32, 0 };
  double labelC[3] = {  19, -32, 0 };
  double labelD[3] = {  19, -22, 0 };

  static double normeye1[3];
  static double normeye2[3];
  static double normeye3[3];
  static double normeye4[3];
  static double normeye5[3];
  static double normeye6[3];
  static double normeye7[3];
  static double normeye8[3];

  static double normeyeA[3];
  static double normeyeB[3];
  static double normeyeC[3];
  static double normeyeD[3];
  static double normeyeE[3];
  static double normeyeF[3];
  static double normeyeG[3];
  static double normeyeH[3];
  static double normeyeI[3];

  static double norm[3];
  static double norm1[3];
  static double norm2[3];
  static double norm3[3];
  static double norm4[3];
  static double norm5[3];
  static double norm6[3];
  static double norm7[3];
  static double norm8[3];
  static double norm9[3];
  static double norm10[3];
  static double norm11[3];
  static double norm12[3];
  static double norm13[3];
  static double norm14[3];
  static double norm15[3];
  static double norm16[3];
  static double norm17[3];
  static double norm18[3];
  static double norm19[3];
  static double norm20[3];
  static double norm21[3];

  static double normA[3];
  static double normB[3];
  static double normC[3];
  static double normD[3];
  static double normE[3];
  static double normF[3];
  static double normG[3];
  static double normH[3];
  static double normI[3];
  static double normJ[3];
  static double normK[3];
  static double normL[3];
  static double normM[3];
  static double normN[3];
  static double normO[3];
  static double normP[3];
  static double normQ[3];
  static double normR[3];
  static double normS[3];
  static double normT[3];
  static double normU[3];

  //R[0] -= fish[fishnum].mouthloc*.3;
  //R[1] -= fish[fishnum].mouthloc;




void draw_fish(int fishnum)
{


//startFishShaders();
  if(!normscalculated){
  	//Create a display list so that it renders faster
  	fishList = glGenLists(2);
  	numberList = glGenLists(128);



  //normals calculted in init
/*    find_normal(T, A, S, norm1);
    find_normal(S, A, B, norm2);
    find_normal(S, B, C, norm3);
    find_normal(S, C, D, norm4);
    find_normal(S, D, E, norm5);
    find_normal(S, E, U, norm6);
    find_normal(S, U, T, norm7);
    find_normal(T, U, M, norm8);
    find_normal(T, M, N, norm9);
    find_normal(T, N, O, norm10);
    find_normal(T, O, P, norm11);
    find_normal(T, P, Q, norm12);
    find_normal(T, Q, R, norm13);
    find_normal(G, H, F, norm14);
    find_normal(H, I, F, norm15);
    find_normal(F, I, E, norm16);
    find_normal(E, I, U, norm17);
    find_normal(U, I, M, norm18);
    find_normal(M, I, L, norm19);
    find_normal(L, I, J, norm20);
    find_normal(L, J, K, norm21);

    find_normal(eyeA, eyeI, eyeB, normeye1);
    find_normal(eyeB, eyeI, eyeC, normeye2);
    find_normal(eyeC, eyeI, eyeD, normeye3);
    find_normal(eyeD, eyeI, eyeE, normeye4);
    find_normal(eyeE, eyeI, eyeF, normeye5);
    find_normal(eyeF, eyeI, eyeG, normeye6);
    find_normal(eyeG, eyeI, eyeH, normeye7);
    find_normal(eyeH, eyeI, eyeA, normeye8);

    normeyeA[0] = (normeye8[0] + normeye1[0])/2;
    normeyeA[1] = (normeye8[1] + normeye1[1])/2;
    normeyeA[2] = (normeye8[2] + normeye1[2])/2;

    normeyeB[0] = (normeye1[0] + normeye2[0])/2;
    normeyeB[1] = (normeye1[1] + normeye2[1])/2;
    normeyeB[2] = (normeye1[2] + normeye2[2])/2;

    normeyeC[0] = (normeye2[0] + normeye3[0])/2;
    normeyeC[1] = (normeye2[1] + normeye3[1])/2;
    normeyeC[2] = (normeye2[2] + normeye3[2])/2;

    normeyeD[0] = (normeye3[0] + normeye4[0])/2;
    normeyeD[1] = (normeye3[1] + normeye4[1])/2;
    normeyeD[2] = (normeye3[2] + normeye4[2])/2;

    normeyeE[0] = (normeye4[0] + normeye5[0])/2;
    normeyeE[1] = (normeye4[1] + normeye5[1])/2;
    normeyeE[2] = (normeye4[2] + normeye5[2])/2;

    normeyeF[0] = (normeye5[0] + normeye6[0])/2;
    normeyeF[1] = (normeye5[1] + normeye6[1])/2;
    normeyeF[2] = (normeye5[2] + normeye6[2])/2;

    normeyeG[0] = (normeye6[0] + normeye7[0])/2;
    normeyeG[1] = (normeye6[1] + normeye7[1])/2;
    normeyeG[2] = (normeye6[2] + normeye7[2])/2;

    normeyeH[0] = (normeye7[0] + normeye8[0])/2;
    normeyeH[1] = (normeye7[1] + normeye8[1])/2;
    normeyeH[2] = (normeye7[2] + normeye8[2])/2;

    normeyeI[0] = (normeye1[0] + normeye2[0] + normeye3[0] + normeye4[0] + normeye5[0] + normeye6[0] + normeye7[0] + normeye8[0])/8;
    normeyeI[1] = (normeye1[1] + normeye2[1] + normeye3[1] + normeye4[1] + normeye5[1] + normeye6[1] + normeye7[1] + normeye8[1])/8;
    normeyeI[2] = (normeye1[2] + normeye2[2] + normeye3[2] + normeye4[2] + normeye5[2] + normeye6[2] + normeye7[2] + normeye8[2])/8;



    normA[0] = (norm1[0] + norm2[0])/2;
    normA[1] = (norm1[1] + norm2[1])/2;
    normA[2] = (norm1[2] + norm2[2])/2;

    normB[0] = (norm2[0] + norm2[0])/2;
    normB[1] = (norm2[1] + norm2[1])/2;
    normB[2] = (norm2[2] + norm2[2])/2;

    normC[0] = (norm3[0] + norm4[0])/2;
    normC[1] = (norm3[1] + norm4[1])/2;
    normC[2] = (norm3[2] + norm4[2])/2;

    normD[0] = (norm4[0] + norm5[0])/2;
    normD[1] = (norm4[1] + norm5[1])/2;
    normD[2] = (norm4[2] + norm5[2])/2;

    normE[0] = (norm5[0] + norm6[0] + norm17[0] + norm16[0])/4;
    normE[1] = (norm5[1] + norm6[1] + norm17[1] + norm16[1])/4;
    normE[2] = (norm5[2] + norm6[2] + norm17[2] + norm16[2])/4;

    normF[0] = (norm16[0] + norm15[0] + norm14[0])/3;
    normF[1] = (norm16[1] + norm15[1] + norm14[1])/3;
    normF[2] = (norm16[2] + norm15[2] + norm14[2])/3;

    normG[0] = norm14[0];
    normG[1] = norm14[1];
    normG[2] = norm14[2];

    normH[0] = (norm14[0] + norm15[0])/2;
    normH[1] = (norm14[1] + norm15[1])/2;
    normH[2] = (norm14[2] + norm15[2])/2;

    normI[0] = (norm15[0] + norm16[0] + norm17[0] + norm18[0] + norm19[0] + norm20[0])/6;
    normI[1] = (norm15[1] + norm16[1] + norm17[1] + norm18[1] + norm19[1] + norm20[1])/6;
    normI[2] = (norm15[2] + norm16[2] + norm17[2] + norm18[2] + norm19[2] + norm20[2])/6;

    normJ[0] = (norm20[0] + norm21[0])/2;
    normJ[1] = (norm20[1] + norm21[1])/2;
    normJ[2] = (norm20[2] + norm21[2])/2;

    normK[0] = norm21[0];
    normK[1] = norm21[1];
    normK[2] = norm21[2];

    normL[0] = (norm21[0] + norm20[0] + norm19[0])/3;
    normL[1] = (norm21[1] + norm20[1] + norm19[1])/3;
    normL[2] = (norm21[2] + norm20[2] + norm19[2])/3;

    normM[0] = (norm19[0] + norm18[0] + norm8[0] + norm9[0])/4;
    normM[1] = (norm19[1] + norm18[1] + norm8[1] + norm9[1])/4;
    normM[2] = (norm19[2] + norm18[2] + norm8[2] + norm9[2])/4;

    normN[0] = (norm9[0] + norm10[0])/2;
    normN[1] = (norm9[1] + norm10[1])/2;
    normN[2] = (norm9[2] + norm10[2])/2;

    normO[0] = (norm10[0] + norm11[0])/2;
    normO[1] = (norm10[1] + norm11[1])/2;
    normO[2] = (norm10[2] + norm11[2])/2;

    normP[0] = (norm11[0] + norm12[0])/2;
    normP[1] = (norm11[1] + norm12[1])/2;
    normP[2] = (norm11[2] + norm12[2])/2;

    normQ[0] = (norm12[0] + norm13[0])/2;
    normQ[1] = (norm12[1] + norm13[1])/2;
    normQ[2] = (norm12[2] + norm13[2])/2;

    normR[0] = norm13[0];
    normR[1] = norm13[1];
    normR[2] = norm13[2];

    normS[0] = (norm1[0] + norm2[0] + norm3[0] + norm4[0] + norm5[0] + norm6[0] + norm7[0])/7;
    normS[1] = (norm1[1] + norm2[1] + norm3[1] + norm4[1] + norm5[1] + norm6[1] + norm7[1])/7;
    normS[2] = (norm1[2] + norm2[2] + norm3[2] + norm4[2] + norm5[2] + norm6[2] + norm7[2])/7;

    normT[0] = (norm1[0] + norm7[0] + norm8[0] + norm9[0] + norm10[0] + norm11[0] + norm12[0] + norm13[0])/8;
    normT[1] = (norm1[1] + norm7[1] + norm8[1] + norm9[1] + norm10[1] + norm11[1] + norm12[1] + norm13[1])/8;
    normT[2] = (norm1[2] + norm7[2] + norm8[2] + norm9[2] + norm10[2] + norm11[2] + norm12[2] + norm13[2])/8;

    normU[0] = (norm6[0] + norm7[0] + norm8[0] + norm18[0] + norm19[0])/5;
    normU[1] = (norm6[1] + norm7[1] + norm8[1] + norm18[1] + norm19[1])/5;
    normU[2] = (norm6[2] + norm7[2] + norm8[2] + norm18[2] + norm19[2])/5;
*/
	//set to 1 after rendering both sides of the fish
    normscalculated = 0;
  }
  //needed to calculate the redness of the fish
	glPushName(fishnum);
	if(!fish[fishnum].clusterPtr->color) {
 fbodyamb[0] = 1; fbodyamb[1] = .6; fbodyamb[2] = 0; fbodyamb[3] =  1.0;
  fbodydif[0] = 1; fbodydif[1] = .6; fbodydif[2] = 0; fbodydif[3] = 1.0;
  fbodyspec[0] = 0.0; fbodyspec[1] = 0.0; fbodyspec[2] = 0.0; fbodyspec[3] = 1.0;
  femissive[0] = 0.0; femissive[1] = 0.0; femissive[2] = 0.0; femissive[3] = 1.0;
	}
	else {
	fbodyamb[0] = fish[fishnum].clusterPtr->color[0]; fbodyamb[1] = fish[fishnum].clusterPtr->color[1]; fbodyamb[2] = fish[fishnum].clusterPtr->color[2]; fbodyamb[3] =  fish[fishnum].clusterPtr->color[3];
	fbodydif[0] = fish[fishnum].clusterPtr->color[4]; fbodydif[1] = fish[fishnum].clusterPtr->color[5]; fbodydif[2] = fish[fishnum].clusterPtr->color[6]; fbodydif[3] = fish[fishnum].clusterPtr->color[7];
	fbodyspec[0] = fish[fishnum].clusterPtr->color[8]; fbodyspec[1] = fish[fishnum].clusterPtr->color[9]; fbodyspec[2] = fish[fishnum].clusterPtr->color[10]; fbodyspec[3] = fish[fishnum].clusterPtr->color[11];
	femissive[0] = fish[fishnum].clusterPtr->color[12]; femissive[1] = fish[fishnum].clusterPtr->color[13]; femissive[2] = fish[fishnum].clusterPtr->color[14]; femissive[3] = fish[fishnum].clusterPtr->color[15];

	}

	//start to turn the fishies red
	/*if(fish[fishnum].size > STARTRED) {
		fbodyamb[1] = .6*((FISHMAXSIZE - fish[fishnum].size)/(FISHMAXSIZE-STARTRED));
		fbodydif[1] = .6*((FISHMAXSIZE - fish[fishnum].size)/(FISHMAXSIZE-STARTRED));

	}
	*/
	#define PI_INCR .866025
	//rotate colors
	if(fish[fishnum].size>STARTCOLOR) {
		//this is here so that this program can run for a long time, without
		//worrying about overflowing a float
		if(fish[fishnum].colorCounter > (2*PI))
			fish[fishnum].colorCounter-=(2*PI);
		fish[fishnum].colorCounter += getTime()*COLORROTATESPEED;
		//code to change the color of the fish
		fbodyamb[0] = ((sin(fish[fishnum].colorCounter)+1)/2)*fabs(1-fish[fishnum].clusterPtr->color[0])+fish[fishnum].clusterPtr->color[0];
		fbodyamb[1] = ((sin(fish[fishnum].colorCounter)+1)/2)*fabs(.6-fish[fishnum].clusterPtr->color[0])+fmin((float)fish[fishnum].clusterPtr->color[1], (float).6);
		fbodyamb[2] = ((sin(fish[fishnum].colorCounter)+1)/2)*fish[fishnum].clusterPtr->color[0];
		fbodyamb[3] = 1.0;
		for(int i = 0; i < 4; i++) {
			fbodydif[i] = fbodyamb[i];
			fbodyspec[i] = fbodyamb[i];

		}
		femissive[0] = femissive[1] = femissive[2] = 0;
		femissive[3] = 1.0;

	}

//		if(fish[fishnum].picked)
//		bodydif[2] = 1.0;
//	else
//		bodydif[2] = 0.0;
	glPushMatrix();
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, fbodyamb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, fbodydif);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, fbodyspec);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, femissive);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);






  glRotated(-90, 0, 1, 0);
  //don't increase the size anymore after FISHMAXSIZE
 	if(fish[fishnum].size > FISHMAXSIZE) {
 		  glScalef((FISHMAXSIZE + 1 + (CPUSTRETCH * (fish[fishnum].cpus -2))) * 1.2 * FISHSCALAR, (FISHMAXSIZE + 1) * 1.2 * FISHSCALAR, (FISHMAXSIZE + 1) *1.2 * FISHSCALAR );
 //		printf("fish %i is %lf\n", fishnum, fish[fishnum].size);
		} else {
		    glScalef((fish[fishnum].size+1.0 + (CPUSTRETCH * (fish[fishnum].cpus -2))) * 1.2 * FISHSCALAR, (fish[fishnum].size+1.0) * 1.2 * FISHSCALAR , (fish[fishnum].size+1.0) * 1.2 * FISHSCALAR );
 	}

/* 	if(fish[fishnum].size > STARTRED) {
 	glPushMatrix();
 	bodyamb[3] = .1;
 	bodydif[3] = .1;
 	bodyspec[3] = .1;
 	emissive[3] = .1;
 	  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, bodyamb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, bodydif);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, bodyspec);

 	if(fish[fishnum].bubbleCounter > 30)
 		fish[fishnum].bubbleCounter = 0;
	glutSolidSphere(fish[fishnum].bubbleCounter+=getTime(), 10, 10);
	glPopMatrix();
	bodyamb[3] = 1.0;
 	bodydif[3] = 1.0;
 	bodyspec[3] = 1.0;
 	emissive[3] = 1.0;
	 	  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, bodyamb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, bodydif);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, bodyspec);

 	}

*/
  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  /*
  glutSolidTeapot(10.0);
  return;
  */
	//  glEnable(GL_CULL_FACE);

   /* --------------------------------------- */
   /* Right-hand Side                         */
   /* --------------------------------------- */

		  glCullFace(GL_BACK);
	    glFrontFace(GL_CCW);
	    static float rightVertArray[117] = {	T[0], T[1], T[2],
									A[0], A[1], A[2],
									S[0], S[1], S[2],

									S[0], S[1], S[2],
									A[0], A[1], A[2],
									B[0], B[1], B[2],

									S[0], S[1], S[2],
									B[0], B[1], B[2],
									C[0], C[1], C[2],

									S[0], S[1], S[2],
									C[0], C[1], C[2],
									D[0], D[1], D[2],

									S[0], S[1], S[2],
									D[0], D[1], D[2],
									E[0], E[1], E[2],

									S[0], S[1], S[2],
									E[0], E[1], E[2],
									U[0], U[1], U[2],

									S[0], S[1], S[2],
									U[0], U[1], U[2],
									T[0], T[1], T[2],

									T[0], T[1], T[2],
									U[0], U[1], U[2],
									M[0], M[1], M[2],

									T[0], T[1], T[2],
									M[0], M[1], M[2],
									N[0], N[1], N[2],

									T[0], T[1], T[2],
									N[0], N[1], N[2],
									O[0], O[1], O[2],

									T[0], T[1], T[2],
									O[0], O[1], O[2],
									P[0], P[1], P[2],

									T[0], T[1], T[2],
									P[0], P[1], P[2],
									Q[0], Q[1], Q[2],

									T[0], T[1], T[2],
									Q[0], Q[1], Q[2],
									R[0], R[1], R[2] };



	static float rightNormArray[117] = {	normT[0], normT[1], normT[2],
									normA[0], normA[1], normA[2],
									normS[0], normS[1], normS[2],

									normS[0], normS[1], normS[2],
									normA[0], normA[1], normA[2],
									normB[0], normB[1], normB[2],

									normS[0], normS[1], normS[2],
									normB[0], normB[1], normB[2],
									normC[0], normC[1], normC[2],

									normS[0], normS[1], normS[2],
									normC[0], normC[1], normC[2],
									normD[0], normD[1], normD[2],

									normS[0], normS[1], normS[2],
									normD[0], normD[1], normD[2],
									normE[0], normE[1], normE[2],

									normS[0], normS[1], normS[2],
									normE[0], normE[1], normE[2],
									normU[0], normU[1], normU[2],

									normS[0], normS[1], normS[2],
									normU[0], normU[1], normU[2],
									normT[0], normT[1], normT[2],

									normT[0], normT[1], normT[2],
									normU[0], normU[1], normU[2],
									normM[0], normM[1], normM[2],

									normT[0], normT[1], normT[2],
									normM[0], normM[1], normM[2],
									normN[0], normN[1], normN[2],

									normT[0], normT[1], normT[2],
									normN[0], normN[1], normN[2],
									normO[0], normO[1], normO[2],

									normT[0], normT[1], normT[2],
									normO[0], normO[1], normO[2],
									normP[0], normP[1], normP[2],

									normT[0], normT[1], normT[2],
									normP[0], normP[1], normP[2],
									normQ[0], normQ[1], normQ[2],

									normT[0], normT[1], normT[2],
									normQ[0], normQ[1], normQ[2],
									normR[0], normR[1], normR[2] };

   glPushMatrix();
   if(!normscalculated) {
	glNewList(fishList, GL_COMPILE);


  glBegin(GL_TRIANGLES);
   // 1
     glNormal3f(normT[0], normT[1], normT[2]);
     glVertex3f(T[0], T[1], T[2]);
     glNormal3f(normA[0], normA[1], normA[2]);
     glVertex3f(A[0], A[1], A[2]);
     glNormal3f(normS[0], normS[1], normS[2]);
     glVertex3f(S[0], S[1], S[2]);

   // 2
     glNormal3f(normS[0], normS[1], normS[2]);
     glVertex3f(S[0], S[1], S[2]);
     glNormal3f(normA[0], normA[1], normA[2]);
     glVertex3f(A[0], A[1], A[2]);
     glNormal3f(normB[0], normB[1], normB[2]);
     glVertex3f(B[0], B[1], B[2]);

   // 3
     glNormal3f(normS[0], normS[1], normS[2]);
     glVertex3f(S[0], S[1], S[2]);
     glNormal3f(normB[0], normB[1], normB[2]);
     glVertex3f(B[0], B[1], B[2]);
     glNormal3f(normC[0], normC[1], normC[2]);
     glVertex3f(C[0], C[1], C[2]);

   // 4
     glNormal3f(normS[0], normS[1], normS[2]);
     glVertex3f(S[0], S[1], S[2]);
     glNormal3f(normC[0], normC[1], normC[2]);
     glVertex3f(C[0], C[1], C[2]);
     glNormal3f(normD[0], normD[1], normD[2]);
    glVertex3f(D[0], D[1], D[2]);

   // 5
     glNormal3f(normS[0], normS[1], normS[2]);
     glVertex3f(S[0], S[1], S[2]);
     glNormal3f(normD[0], normD[1], normD[2]);
     glVertex3f(D[0], D[1], D[2]);
     glNormal3f(normE[0], normE[1], normE[2]);
     glVertex3f(E[0], E[1], E[2]);

   // 6
     glNormal3f(normS[0], normS[1], normS[2]);
     glVertex3f(S[0], S[1], S[2]);
     glNormal3f(normE[0], normE[1], normE[2]);
     glVertex3f(E[0], E[1], E[2]);
     glNormal3f(normU[0], normU[1], normU[2]);
     glVertex3f(U[0], U[1], U[2]);

   // 7
     glNormal3f(normS[0], normS[1], normS[2]);
     glVertex3f(S[0], S[1], S[2]);
     glNormal3f(normU[0], normU[1], normU[2]);
     glVertex3f(U[0], U[1], U[2]);
     glNormal3f(normT[0], normT[1], normT[2]);
     glVertex3f(T[0], T[1], T[2]);

   // 8
     glNormal3f(normT[0], normT[1], normT[2]);
     glVertex3f(T[0], T[1], T[2]);
     glNormal3f(normU[0], normU[1], normU[2]);
     glVertex3f(U[0], U[1], U[2]);
     glNormal3f(normM[0], normM[1], normM[2]);
     glVertex3f(M[0], M[1], M[2]);

   // 9
     glNormal3f(normT[0], normT[1], normT[2]);
     glVertex3f(T[0], T[1], T[2]);
     glNormal3f(normM[0], normM[1], normM[2]);
     glVertex3f(M[0], M[1], M[2]);
     glNormal3f(normN[0], normN[1], normN[2]);
     glVertex3f(N[0], N[1], N[2]);

   // 10
     glNormal3f(normT[0], normT[1], normT[2]);
     glVertex3f(T[0], T[1], T[2]);
     glNormal3f(normN[0], normN[1], normN[2]);
     glVertex3f(N[0], N[1], N[2]);
     glNormal3f(normO[0], normO[1], normO[2]);
     glVertex3f(O[0], O[1], O[2]);

   // 11
     glNormal3f(normT[0], normT[1], normT[2]);
     glVertex3f(T[0], T[1], T[2]);
     glNormal3f(normO[0], normO[1], normO[2]);
     glVertex3f(O[0], O[1], O[2]);
     glNormal3f(normP[0], normP[1], normP[2]);
     glVertex3f(P[0], P[1], P[2]);

   // 12
     glNormal3f(normT[0], normT[1], normT[2]);
     glVertex3f(T[0], T[1], T[2]);
     glNormal3f(normP[0], normP[1], normP[2]);
     glVertex3f(P[0], P[1], P[2]);
     glNormal3f(normQ[0], normQ[1], normQ[2]);
     glVertex3f(Q[0], Q[1], Q[2]);

   // 13
     glNormal3f(normT[0], normT[1], normT[2]);
     glVertex3f(T[0], T[1], T[2]);
     glNormal3f(normQ[0], normQ[1], normQ[2]);
     glVertex3f(Q[0], Q[1], Q[2]);
     glNormal3f(normR[0], normR[1], normR[2]);
     glVertex3f(R[0], R[1], R[2]);

   glEnd();

   glEndList();
/*the point of this section is to make a display list for each number to display*/

//texture number stuff

	double tts[] = {0.0, 0.0, 0.0};
	double tty[] = {0.0, 20.0, 0.0};
	double ttz[] = {20.0, 20.0, 0.0};
	double tmp[3];
	find_normal(tts, tty, ttz, tmp);

for(int i = 0; i < 128; i++) {
	glNewList(numberList+i, GL_COMPILE);
   	glPushMatrix();
   glEnable(GL_TEXTURE_2D);
   glEnable(GL_BLEND);
 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifndef WIN32
   	glBindTexture(GL_TEXTURE_2D,font.textures[i]);
	glBegin(GL_QUADS);

		//normalize the quad.  notice the function is designed with a triangle in mind, but if you send
		//it 3 points on the quad, it can find the normal anyways, quad = 2 triangles
		glNormal3d(tmp[0], tmp[1], tmp[2]);
		glTexCoord2d(0.0, 1.0); glVertex3d(0.0, 0.0, 0.0);
		glTexCoord2d(0.0, 0.0); glVertex3d(0.0, 20.0, 0.0);
		glTexCoord2d(1.0, 0.0); glVertex3d(20.0, 20.0, 0.0);
		glTexCoord2d(1.0, 1.0); glVertex3d(20.0, 0.0, 0.0);
	glEnd();
#endif
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glEndList();

}



	} else {


	#ifdef VBOS
	glPushMatrix();
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	if( VBOSupported )
	{
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, VBOVertices );
		glVertexPointer( 3, GL_FLOAT, 0, (char *) NULL );		// Set The Vertex Pointer To The Vertex Buffer
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, VBONormals );
		glNormalPointer(GL_FLOAT, 0, (char *) NULL );		// Set The TexCoord Pointer To The TexCoord Buffer

	} else
	{
		glVertexPointer( 3, GL_FLOAT, 0, rightVertArray );	// Set The Vertex Pointer To Our Vertex Data
		glNormalPointer(GL_FLOAT, 0, rightNormArray );	// Set The Vertex Pointer To Our TexCoord Data
	}

	glDrawArrays(GL_TRIANGLES, 0, 39);
	//glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);
	//glBindBufferARB( GL_ARRAY_BUFFER_ARB, VBONormalsLeft );
	//glNormalPointer(GL_FLOAT, 0, rightNormArray);
	//glVertexPointer(3, GL_FLOAT, 0, rightVertArray);
	//glScalef(1.0, 1.0, -1.0);
	//glDrawArrays(GL_TRIANGLES, 0, 39);
	glDisableClientState(GL_NORMAL_ARRAY);


	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
  #else
  glCallList(fishList);
  #endif
	if(fish[fishnum].alive == 0 && !picking) {

		glPushMatrix();
		glTranslatef(0, 0, -10);
		fish[fishnum].deadcounter += getTime() *(360/ROTATIONTIME);
		fish[fishnum].deadcounter = fmod(fish[fishnum].deadcounter, 360);
		glRotated(-90, 1, 0, 0);
		glRotated(fish[fishnum].deadcounter, 0, 1, 0);



		if(!fish[fishnum].deadnumberList) {

		fish[fishnum].deadnumberList = glGenLists(1);
		glNewList(fish[fishnum].deadnumberList, GL_COMPILE);

		glScalef(.5, .5, .5);

		glPushMatrix();

		glTranslatef(25, 0, 0);
		glRotated(90, 0, 1, 0);
		glCallList(numberList+fish[fishnum].print[0]);
		glTranslatef(20, 0, 0);
		glRotated(50, 0, 1, 0);
		glCallList(numberList+fish[fishnum].print[1]);
		glTranslatef(20, 0, 0);
		glRotated(50, 0, 1, 0);
		glCallList(numberList+fish[fishnum].print[2]);


		glPopMatrix();

		glPushMatrix();

		glTranslatef(-25, 0, 0);
		glRotated(-90, 0, 1, 0);
		glCallList(numberList+fish[fishnum].print[0]);
		glTranslatef(20, 0, 0);
		glRotated(50, 0, 1, 0);
		glCallList(numberList+fish[fishnum].print[1]);
		glTranslatef(20, 0, 0);
		glRotated(50, 0, 1, 0);
		glCallList(numberList+fish[fishnum].print[2]);


		glPopMatrix();
		glEndList();
		} else {
			glCallList(fish[fishnum].deadnumberList);
		}

		glPopMatrix();
	}
	else if(fish[fishnum].size > STARTNUMBER && !picking) {


//	for(int c = 0; c < 3; c++) {
//	if(fish[fishnum].print[c] > 57 || fish[fishnum].print[c] < 48)
//		printf("%s has a invalid print[%i] value: %c, integer = %i\n", fish[fishnum].fishname, c, fish[fishnum].print[c], (int)fish[fishnum].print[c]);
//	}
	/*
	//right side

	if(!fish[fishnum].alivenumberList) {

	fish[fishnum].alivenumberList = glGenLists(1);
	glNewList(fish[fishnum].alivenumberList, GL_COMPILE);
	glPushMatrix();



	glScalef(.5, .5, .5);
	glTranslatef(-22, -20, 6.5);
		glRotated(11, 1, 0, 0);
	glRotated(-30, 0, 1, 0);
	glCallList(numberList+fish[fishnum].print[0]);
	glTranslatef(20, 0, 0);
	glRotated(25, 0, 1, 0);
	glCallList(numberList+fish[fishnum].print[1]);
	glTranslatef(20, 0, 0);
	glRotated(16, 0, 1, 0);
	glCallList(numberList+fish[fishnum].print[2]);

	glPopMatrix();



	//left side
	glPushMatrix();
	glRotated(180, 0, 1, 0);



	glScalef(.5, .5, .5);
	//weird that the x of the translate isn't the same as the before, but whatever
	glTranslatef(-36, -20, 14);
	glRotated(10, 1, 0, 0);
	glRotated(-12, 0, 1, 0);
	glCallList(numberList+fish[fishnum].print[0]);
	glTranslatef(20, 0, 0);
	glRotated(20, 0, 1, 0);
	glCallList(numberList+fish[fishnum].print[1]);
	glTranslatef(20, 0, 0);
	glRotated(22, 0, 1, 0);
	glCallList(numberList+fish[fishnum].print[2]);

	glPopMatrix();
	glEndList();
	} else {
		glCallList(fish[fishnum].alivenumberList);
	}
	}

	*/
	   }
	}

	glDisable(GL_CULL_FACE);
   glPopMatrix();

   glPushMatrix();
   glTranslated(U[0], 0, 0);
   glRotated(fish[fishnum].finrotation, 0, 1, 0);
   glTranslated(-U[0], 0, 0);

   /* Tail */
   glPushMatrix();


   /* 16 */
   glPushMatrix();
   glTranslated(E[0], E[1], E[2]);
   glRotated(0.5*fish[fishnum].oldfinrotation[0], I[0]-E[0], I[1]-E[1], I[2]-E[2]);
   glTranslated(-E[0], -E[1], -E[2]);

	glCallList(tailLists);
 /*  glBegin(GL_TRIANGLES);
     glNormal3f(normF[0], normF[1], normF[2]);
     glVertex3f(F[0], F[1], F[2]);
     glNormal3f(normI[0], normI[1], normI[2]);
     glVertex3f(I[0], I[1], I[2]);
     glNormal3f(normE[0], normE[1], normE[2]);
     glVertex3f(E[0], E[1], E[2]);

     /*
     glNormal3f(normF[0], normF[1], normF[2]);
     glVertex3f(F[0], F[1], F[2]);
     glNormal3f(normI[0], normI[1], normI[2]);
     glVertex3f(I[0], I[1], I[2]);
     glNormal3f(normE[0], normE[1], normE[2]);
     glVertex3f(E[0], E[1], E[2]);
     */
//   glEnd();


   /* 15 */
//   glPushMatrix();
//   glTranslated(F[0], F[1], F[2]);
   glRotated(0.75*fish[fishnum].oldfinrotation[1], I[0]-F[0], I[1]-F[1], I[2]-F[2]);
//   glTranslated(-F[0], -F[1], -F[2]);

	glCallList(tailLists+1);
  /* glBegin(GL_TRIANGLES);
     glNormal3f(normH[0], normH[1], normH[2]);
     glVertex3f(H[0], H[1], H[2]);
     glNormal3f(normI[0], normI[1], normI[2]);
     glVertex3f(I[0], I[1], I[2]);
     glNormal3f(normF[0], normF[1], normF[2]);
     glVertex3f(F[0], F[1], F[2]);
   glEnd();
*/
//   glPushMatrix();
//   glTranslated(F[0], F[1], F[2]);
   glRotated(1.0*fish[fishnum].oldfinrotation[2], H[0]-F[0], H[1]-F[1], H[2]-F[2]);
//   glTranslated(-F[0], -F[1], -F[2]);

   /* 14 */

   glCallList(tailLists+2);
/*   glBegin(GL_TRIANGLES);
     glNormal3f(normG[0], normG[1], normG[2]);
     glVertex3f(G[0], G[1], G[2]);
     glNormal3f(normH[0], normH[1], normH[2]);
     glVertex3f(H[0], H[1], H[2]);
     glNormal3f(normF[0], normF[1], normF[2]);
     glVertex3f(F[0], F[1], F[2]);
   glEnd();

   glPopMatrix();
   glPopMatrix();
   glPopMatrix();


   /* 17 */
   glBegin(GL_TRIANGLES);
     glNormal3f(normE[0], normE[1], normE[2]);
     glVertex3f(E[0], E[1], E[2]);
     glNormal3f(normI[0], normI[1], normI[2]);
     glVertex3f(I[0], I[1], I[2]);
     glNormal3f(normU[0], normU[1], normU[2]);
     //glVertex3f(U[0], U[1], U[2]);
     //glVertex3f(U[0]+cos(fish[fishnum].rotation*(degrad)), U[1], U[2]+sin(fish[fishnum].rotation*(degrad)));
     glVertex3f(U[2]*(-sin(fish[fishnum].finrotation*(degrad)))+U[0], U[1], U[2]*cos(fish[fishnum].finrotation*(degrad)));
   glEnd();

   /* 18 */
   glBegin(GL_TRIANGLES);
     glNormal3f(normU[0], normU[1], normU[2]);
     //glVertex3f(U[0], U[1], U[2]);
     //glVertex3f(U[0]+cos(fish[fishnum].rotation*(degrad)), U[1], U[2]+sin(fish[fishnum].rotation*(degrad)));
     glVertex3f(U[2]*(-sin(fish[fishnum].finrotation*(degrad)))+U[0], U[1], U[2]*cos(fish[fishnum].finrotation*(degrad)));
     glNormal3f(normI[0], normI[1], normI[2]);
     glVertex3f(I[0], I[1], I[2]);
     glNormal3f(normM[0], normM[1], normM[2]);
     glVertex3f(M[0], M[1], M[2]);
   glEnd();


   /* 19 */
   glPushMatrix();
   glTranslated(M[0], M[1], M[2]);
   glRotated(-(1.0)*fish[fishnum].oldfinrotation[0], I[0]-M[0], I[1]-M[1], I[2]-M[2]);
 //  glTranslated(-M[0], -M[1], -M[2]);

   glCallList(tailLists+3);

 /* glBegin(GL_TRIANGLES);
     glNormal3f(normM[0], normM[1], normM[2]);
     glVertex3f(M[0], M[1], M[2]);
     glNormal3f(normI[0], normI[1], normI[2]);
     glVertex3f(I[0], I[1], I[2]);
     glNormal3f(normL[0], normL[1], normL[2]);
     glVertex3f(L[0], L[1], L[2]);
   glEnd();

   /* 20 */
  //glPushMatrix();
   //glTranslated(L[0], L[1], L[2]);
   glRotated(-(0.75)*fish[fishnum].oldfinrotation[1], I[0]-L[0], I[1]-L[1], I[2]-L[2]);
  // glTranslated(-L[0], -L[1], -L[2]);

   glCallList(tailLists+4);
/*
   glBegin(GL_TRIANGLES);
     glNormal3f(normL[0], normL[1], normL[2]);
     glVertex3f(L[0], L[1], L[2]);
     glNormal3f(normI[0], normI[1], normI[2]);
     glVertex3f(I[0], I[1], I[2]);
     glNormal3f(normK[0], normK[1], normK[2]);
     glVertex3f(J[0], J[1], J[2]);
   glEnd();

   /* 21 */
   //glPushMatrix();
   //glTranslated(L[0], L[1], L[2]);
   glRotated(-(0.5)*fish[fishnum].oldfinrotation[2], J[0]-L[0], J[1]-L[1], J[2]-L[2]);
 //  glTranslated(-L[0], -L[1], -L[2]);

   glCallList(tailLists+5);

/*   glBegin(GL_TRIANGLES+5);
     glNormal3f(normL[0], normL[1], normL[2]);
     glVertex3f(L[0], L[1], L[2]);
     glNormal3f(normJ[0], normJ[1], normJ[2]);
     glVertex3f(J[0], J[1], J[2]);
     glNormal3f(normK[0], normK[1], normK[2]);
     glVertex3f(K[0], K[1], K[2]);
   glEnd();

   glPopMatrix();
   glPopMatrix();
   glPopMatrix();


   glPopMatrix();


   glPopMatrix();
*/


   /* Eye */
   if(fish[fishnum].alive){
   glPushMatrix();
   /*
    *   GLfloat bodyamb[] = { 1, .6, 0, 1.0 };
  GLfloat bodydif[] = { 1, .6, 0, 1.0 };
  GLfloat bodyspec[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat emissive[] = { 0.0, 0.0, 0.0, 1.0 };
  */
	float eyecloseamb[] = { 1, .7, 0, 1.0 };
	float eyeclosedif[] = { 1, .7, 0, 1.0 };
	float eyeclosespec[] = {0.0, 0.0, 0.0, 1.0 };

     if(fish[fishnum].blinking == EYE_OPEN){
       glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, eyeamb);
       glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, eyedif);
       glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, eyespec);
     }else{
       glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, fbodyamb);
       glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, fbodydif);
       glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, fbodyspec);
     }
    glCallList(eyeLists+5);
/*     glTranslated(eyeI[0], eyeI[1], eyeI[2]);
     glRotated(5, -1, 0, 0);
     glRotated(29, 0, 1, 0);
     glTranslated(-eyeI[0], -eyeI[1], -(eyeI[2]));

     glBegin(GL_TRIANGLES);

       glNormal3f(normeyeA[0], normeyeA[1], normeyeA[2]);
       glVertex3f(eyeA[0], eyeA[1], eyeA[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeB[0], normeyeB[1], normeyeB[2]);
       glVertex3f(eyeB[0], eyeB[1], eyeB[2]);

       glNormal3f(normeyeB[0], normeyeB[1], normeyeB[2]);
       glVertex3f(eyeB[0], eyeB[1], eyeB[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeC[0], normeyeC[1], normeyeC[2]);
       glVertex3f(eyeC[0], eyeC[1], eyeC[2]);

       glNormal3f(normeyeC[0], normeyeC[1], normeyeC[2]);
       glVertex3f(eyeC[0], eyeC[1], eyeC[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeD[0], normeyeD[1], normeyeD[2]);
       glVertex3f(eyeD[0], eyeD[1], eyeD[2]);

       glNormal3f(normeyeG[0], normeyeG[1], normeyeG[2]);
       glVertex3f(eyeG[0], eyeG[1], eyeG[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeH[0], normeyeH[1], normeyeH[2]);
       glVertex3f(eyeH[0], eyeH[1], eyeH[2]);

       glNormal3f(normeyeH[0], normeyeH[1], normeyeH[2]);
       glVertex3f(eyeH[0], eyeH[1], eyeH[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeA[0], normeyeA[1], normeyeA[2]);
       glVertex3f(eyeA[0], eyeA[1], eyeA[2]);

     glEnd();

     /* Bottom half */
     if((fish[fishnum].blinking == EYE_HALF_OPEN) || (fish[fishnum].blinking == EYE_HALF_CLOSED)){
       glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, eyeamb);
       glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, eyedif);
       glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, eyespec);
       glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, femissive);
       glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
     }
	glCallList(eyeLists+6);
/*     glBegin(GL_TRIANGLES);

       glNormal3f(normeyeD[0], normeyeD[1], normeyeD[2]);
       glVertex3f(eyeD[0], eyeD[1], eyeD[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeE[0], normeyeE[1], normeyeE[2]);
       glVertex3f(eyeE[0], eyeE[1], eyeE[2]);

       glNormal3f(normeyeE[0], normeyeE[1], normeyeE[2]);
       glVertex3f(eyeE[0], eyeE[1], eyeE[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeF[0], normeyeF[1], normeyeF[2]);
       glVertex3f(eyeF[0], eyeF[1], eyeF[2]);

       glNormal3f(normeyeF[0], normeyeF[1], normeyeF[2]);
       glVertex3f(eyeF[0], eyeF[1], eyeF[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeG[0], normeyeG[1], normeyeG[2]);
       glVertex3f(eyeG[0], eyeG[1], eyeG[2]);

     glEnd();

     //glPopMatrix();

     //glPushMatrix();

     glShadeModel(GL_FLAT);
     /* Pupil */
     if(fish[fishnum].blinking != EYE_CLOSED){
       glTranslated(eyeI[0], eyeI[1], eyeI[2]+.1);
       glScalef(.2, .2, .2);
       glTranslated(-eyeI[0], -eyeI[1], -eyeI[2]);

       glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pupilamb);
       glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pupildif);
       glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pupilspec);
       glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, femissive);
       glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);

       if(fish[fishnum].blinking == EYE_OPEN){
       	glCallList(eyeLists+7);
/*	 glBegin(GL_TRIANGLES);

	 glNormal3f(normeyeA[0], normeyeA[1], normeyeA[2]);
	 glVertex3f(eyeA[0], eyeA[1], eyeA[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]+1);
	 glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
	 glNormal3f(normeyeB[0], normeyeB[1], normeyeB[2]);
	 glVertex3f(eyeB[0], eyeB[1], eyeB[2]);


	 glNormal3f(normeyeB[0], normeyeB[1], normeyeB[2]);
	 glVertex3f(eyeB[0], eyeB[1], eyeB[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]+1);
	 glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
	 glNormal3f(normeyeC[0], normeyeC[1], normeyeC[2]);
	 glVertex3f(eyeC[0], eyeC[1], eyeC[2]);

	 glNormal3f(normeyeC[0], normeyeC[1], normeyeC[2]);
	 glVertex3f(eyeC[0], eyeC[1], eyeC[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]+1);
	 glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
	 glNormal3f(normeyeD[0], normeyeD[1], normeyeD[2]);
	 glVertex3f(eyeD[0], eyeD[1], eyeD[2]);

	 glNormal3f(normeyeG[0], normeyeG[1], normeyeG[2]);
	 glVertex3f(eyeG[0], eyeG[1], eyeG[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]+1);
	 glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
	 glNormal3f(normeyeH[0], normeyeH[1], normeyeH[2]);
	 glVertex3f(eyeH[0], eyeH[1], eyeH[2]);

	 glNormal3f(normeyeH[0], normeyeH[1], normeyeH[2]);
	 glVertex3f(eyeH[0], eyeH[1], eyeH[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]+1);
	 glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
	 glNormal3f(normeyeA[0], normeyeA[1], normeyeA[2]);
	 glVertex3f(eyeA[0], eyeA[1], eyeA[2]);

	 glEnd();
*/       }
	glCallList(eyeLists+8);
/*       glBegin(GL_TRIANGLES);

       glNormal3f(normeyeD[0], normeyeD[1], normeyeD[2]);
       glVertex3f(eyeD[0], eyeD[1], eyeD[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]+1);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeE[0], normeyeE[1], normeyeE[2]);
       glVertex3f(eyeE[0], eyeE[1], eyeE[2]);

       glNormal3f(normeyeE[0], normeyeE[1], normeyeE[2]);
       glVertex3f(eyeE[0], eyeE[1], eyeE[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]+1);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeF[0], normeyeF[1], normeyeF[2]);
       glVertex3f(eyeF[0], eyeF[1], eyeF[2]);

       glNormal3f(normeyeF[0], normeyeF[1], normeyeF[2]);
       glVertex3f(eyeF[0], eyeF[1], eyeF[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]+1);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeG[0], normeyeG[1], normeyeG[2]);
       glVertex3f(eyeG[0], eyeG[1], eyeG[2]);

       glEnd();

 */    }
     glShadeModel(GL_SMOOTH);

     glPopMatrix();
   }
   else{
     /* Dead Eye */
     glCallList(eyeLists+9);
/*     glShadeModel(GL_FLAT);
     glPushMatrix();
     glTranslated(eyeI[0], eyeI[1], eyeI[2]-3);
     glRotated(9.5, -1, 0, 0);
     glRotated(24, 0, 1, 0);
     glTranslated(-eyeI[0], -eyeI[1], -(eyeI[2]-3));

     glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pupilamb);
     glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pupildif);
     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pupilspec);
     glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive);
     glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);

     glBegin(GL_QUADS);
       glVertex3f(deadeyeA[0], deadeyeA[1], deadeyeA[2]);
       glVertex3f(deadeyeH[0], deadeyeH[1], deadeyeH[2]);
       glVertex3f(deadeyeE[0], deadeyeE[1], deadeyeE[2]);
       glVertex3f(deadeyeD[0], deadeyeD[1], deadeyeD[2]);

       glVertex3f(deadeyeC[0], deadeyeC[1], deadeyeC[2]);
       glVertex3f(deadeyeB[0], deadeyeB[1], deadeyeB[2]);
       glVertex3f(deadeyeG[0], deadeyeG[1], deadeyeG[2]);
       glVertex3f(deadeyeF[0], deadeyeF[1], deadeyeF[2]);
     glEnd();
     glPopMatrix();
     glShadeModel(GL_SMOOTH);
*/   }

   /* --------------------------------------- */
   /* Left-hand Side                          */
   /* --------------------------------------- */

   glFrontFace(GL_CW);

   glPushMatrix();

   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, fbodyamb);
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, fbodydif);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, fbodyspec);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, femissive);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);

	if(!normscalculated) {
	glNewList(fishList+1, GL_COMPILE);
   glBegin(GL_TRIANGLES);
   /* 1 */
     glNormal3f(normT[0], normT[1], -normT[2]);
     glVertex3f(T[0], T[1], -T[2]);
     glNormal3f(normA[0], normA[1], -normA[2]);
     glVertex3f(A[0], A[1], -A[2]);
     glNormal3f(normS[0], normS[1], -normS[2]);
     glVertex3f(S[0], S[1], -S[2]);

   /* 2 */
     glNormal3f(normS[0], normS[1], -normS[2]);
     glVertex3f(S[0], S[1], -S[2]);
     glNormal3f(normA[0], normA[1], -normA[2]);
     glVertex3f(A[0], A[1], -A[2]);
     glNormal3f(normB[0], normB[1], -normB[2]);
     glVertex3f(B[0], B[1], -B[2]);

   /* 3 */
     glNormal3f(normS[0], normS[1], -normS[2]);
     glVertex3f(S[0], S[1], -S[2]);
     glNormal3f(normB[0], normB[1], -normB[2]);
     glVertex3f(B[0], B[1], -B[2]);
     glNormal3f(normC[0], normC[1], -normC[2]);
     glVertex3f(C[0], C[1], -C[2]);

   /* 4 */
     glNormal3f(normS[0], normS[1], -normS[2]);
     glVertex3f(S[0], S[1], -S[2]);
     glNormal3f(normC[0], normC[1], -normC[2]);
     glVertex3f(C[0], C[1], -C[2]);
     glNormal3f(normD[0], normD[1], -normD[2]);
     glVertex3f(D[0], D[1], -D[2]);

   /* 5 */
     glNormal3f(normS[0], normS[1], -normS[2]);
     glVertex3f(S[0], S[1], -S[2]);
     glNormal3f(normD[0], normD[1], -normD[2]);
     glVertex3f(D[0], D[1], -D[2]);
     glNormal3f(normE[0], normE[1], -normE[2]);
     glVertex3f(E[0], E[1], -E[2]);

   /* 6 */
     glNormal3f(normS[0], normS[1], -normS[2]);
     glVertex3f(S[0], S[1], -S[2]);
     glNormal3f(normE[0], normE[1], -normE[2]);
     glVertex3f(E[0], E[1], -E[2]);
     glNormal3f(normU[0], normU[1], -normU[2]);
     glVertex3f(U[0], U[1], -U[2]);

   /* 7 */
     glNormal3f(normS[0], normS[1], -normS[2]);
     glVertex3f(S[0], S[1], -S[2]);
     glNormal3f(normU[0], normU[1], -normU[2]);
     glVertex3f(U[0], U[1], -U[2]);
     glNormal3f(normT[0], normT[1], -normT[2]);
     glVertex3f(T[0], T[1], -T[2]);

   /* 8 */
     glNormal3f(normT[0], normT[1], -normT[2]);
     glVertex3f(T[0], T[1], -T[2]);
     glNormal3f(normU[0], normU[1], -normU[2]);
     glVertex3f(U[0], U[1], -U[2]);
     glNormal3f(normM[0], normM[1], -normM[2]);
     glVertex3f(M[0], M[1], -M[2]);

   /* 9 */
     glNormal3f(normT[0], normT[1], -normT[2]);
     glVertex3f(T[0], T[1], -T[2]);
     glNormal3f(normM[0], normM[1], -normM[2]);
     glVertex3f(M[0], M[1], -M[2]);
     glNormal3f(normN[0], normN[1], -normN[2]);
     glVertex3f(N[0], N[1], -N[2]);

   /* 10 */
     glNormal3f(normT[0], normT[1], -normT[2]);
     glVertex3f(T[0], T[1], -T[2]);
     glNormal3f(normN[0], normN[1], -normN[2]);
     glVertex3f(N[0], N[1], -N[2]);
     glNormal3f(normO[0], normO[1], -normO[2]);
     glVertex3f(O[0], O[1], -O[2]);

   /* 11 */
     glNormal3f(normT[0], normT[1], -normT[2]);
     glVertex3f(T[0], T[1], -T[2]);
     glNormal3f(normO[0], normO[1], -normO[2]);
     glVertex3f(O[0], O[1], -O[2]);
     glNormal3f(normP[0], normP[1], -normP[2]);
     glVertex3f(P[0], P[1], -P[2]);

   /* 12 */
     glNormal3f(normT[0], normT[1], -normT[2]);
     glVertex3f(T[0], T[1], -T[2]);
     glNormal3f(normP[0], normP[1], -normP[2]);
     glVertex3f(P[0], P[1], -P[2]);
     glNormal3f(normQ[0], normQ[1], -normQ[2]);
     glVertex3f(Q[0], Q[1], -Q[2]);

   /* 13 */
     glNormal3f(normT[0], normT[1], -normT[2]);
     glVertex3f(T[0], T[1], -T[2]);
     glNormal3f(normQ[0], normQ[1], -normQ[2]);
     glVertex3f(Q[0], Q[1], -Q[2]);
     glNormal3f(normR[0], normR[1], -normR[2]);
     glVertex3f(R[0], R[1], -R[2]);

   glEnd();
   glEndList();
   normscalculated = 1;
	} else {
		glCallList(fishList+1);
	}

   glPopMatrix();

   glPushMatrix();
   glTranslated(U[0], 0, 0);
   glRotated(fish[fishnum].finrotation, 0, 1, 0);
   glTranslated(-U[0], 0, 0);

   /* 14 */
   /*
   glBegin(GL_TRIANGLES);
     glNormal3f(normG[0], normG[1], -normG[2]);
     glVertex3f(G[0], G[1], -G[2]);
     glNormal3f(normH[0], normH[1], -normH[2]);
     glVertex3f(H[0], H[1], -H[2]);
     glNormal3f(normF[0], normF[1], -normF[2]);
     glVertex3f(F[0], F[1], -F[2]);
   glEnd();
   */
   /* 15 */
   /*
   glBegin(GL_TRIANGLES);
     glNormal3f(normH[0], normH[1], -normH[2]);
     glVertex3f(H[0], H[1], -H[2]);
     glNormal3f(normI[0], normI[1], -normI[2]);
     glVertex3f(I[0], I[1], -I[2]);
     glNormal3f(normF[0], normF[1], -normF[2]);
     glVertex3f(F[0], F[1], -F[2]);
   glEnd();
   */
   /* 16 */
   /*
   glBegin(GL_TRIANGLES);
     glNormal3f(normF[0], normF[1], -normF[2]);
     glVertex3f(F[0], F[1], -F[2]);
     glNormal3f(normI[0], normI[1], -normI[2]);
     glVertex3f(I[0], I[1], -I[2]);
     glNormal3f(normE[0], normE[1], -normE[2]);
     glVertex3f(E[0], E[1], -E[2]);
   glEnd();
   */
   /* 17 */
   glBegin(GL_TRIANGLES);
     glNormal3f(normE[0], normE[1], -normE[2]);
     glVertex3f(E[0], E[1], -E[2]);
     glNormal3f(normI[0], normI[1], -normI[2]);
     glVertex3f(I[0], I[1], -I[2]);
     glNormal3f(normU[0], normU[1], -normU[2]);
     //glVertex3f(U[0], U[1], -U[2]);
     //glVertex3f(U[0]+cos(fish[fishnum].rotation*(degrad)), U[1], -(U[2]+sin(fish[fishnum].rotation*(degrad))));
     glVertex3f(-U[2]*(-sin(fish[fishnum].finrotation*(degrad)))+U[0], U[1], -U[2]*cos(fish[fishnum].finrotation*(degrad)));
   glEnd();

   /* 18 */
   glBegin(GL_TRIANGLES);
     glNormal3f(normU[0], normU[1], -normU[2]);
     //glVertex3f(U[0], U[1], -U[2]);
     //glVertex3f(U[0]+cos(fish[fishnum].rotation*(degrad)), U[1], -(U[2]+sin(fish[fishnum].rotation*(degrad))));
     glVertex3f(-U[2]*(-sin(fish[fishnum].finrotation*(degrad)))+U[0], U[1], -U[2]*cos(fish[fishnum].finrotation*(degrad)));
     glNormal3f(normI[0], normI[1], -normI[2]);
     glVertex3f(I[0], I[1], -I[2]);
     glNormal3f(normM[0], normM[1], -normM[2]);
     glVertex3f(M[0], M[1], -M[2]);
   glEnd();

   /* 19 */
   /*
   glBegin(GL_TRIANGLES);
     glNormal3f(normM[0], normM[1], -normM[2]);
     glVertex3f(M[0], M[1], -M[2]);
     glNormal3f(normI[0], normI[1], -normI[2]);
     glVertex3f(I[0], I[1], -I[2]);
     glNormal3f(normL[0], normL[1], -normL[2]);
     glVertex3f(L[0], L[1], -L[2]);
   glEnd();
   */

   /* 20 */
   /*
   glBegin(GL_TRIANGLES);
     glNormal3f(normL[0], normL[1], -normL[2]);
     glVertex3f(L[0], L[1], -L[2]);
     glNormal3f(normI[0], normI[1], -normI[2]);
     glVertex3f(I[0], I[1], -I[2]);
     glNormal3f(normK[0], normK[1], -normK[2]);
     glVertex3f(J[0], J[1], -J[2]);
   glEnd();
   */

   /* 21 */
   /*
   glBegin(GL_TRIANGLES);
     glNormal3f(normL[0], normL[1], -normL[2]);
     glVertex3f(L[0], L[1], -L[2]);
     glNormal3f(normJ[0], normJ[1], -normJ[2]);
     glVertex3f(J[0], J[1], -J[2]);
     glNormal3f(normK[0], normK[1], -normK[2]);
     glVertex3f(K[0], K[1], -K[2]);
   glEnd();
   */
   glPopMatrix();

   /* Eye */
   if(fish[fishnum].alive){
   glPushMatrix();

     if(fish[fishnum].blinking == EYE_OPEN){
       glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, eyeamb);
       glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, eyedif);
       glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, eyespec);
     }else{
       glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, fbodyamb);
       glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, fbodydif);
       glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, fbodyspec);
     }
	glCallList(eyeLists);
/*
     glTranslated(eyeI[0], eyeI[1], -eyeI[2]);
     glRotated(5, 1, 0, 0);
     glRotated(29, 0, -1, 0);
     glTranslated(-eyeI[0], -eyeI[1], (eyeI[2]));


     glBegin(GL_TRIANGLES);

       glNormal3f(normeyeA[0], normeyeA[1], -normeyeA[2]);
       glVertex3f(eyeA[0], eyeA[1], -eyeA[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeB[0], normeyeB[1], -normeyeB[2]);
       glVertex3f(eyeB[0], eyeB[1], -eyeB[2]);

       glNormal3f(normeyeB[0], normeyeB[1], -normeyeB[2]);
       glVertex3f(eyeB[0], eyeB[1], -eyeB[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeC[0], normeyeC[1], -normeyeC[2]);
       glVertex3f(eyeC[0], eyeC[1], -eyeC[2]);

       glNormal3f(normeyeC[0], normeyeC[1], -normeyeC[2]);
       glVertex3f(eyeC[0], eyeC[1], -eyeC[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeD[0], normeyeD[1], -normeyeD[2]);
       glVertex3f(eyeD[0], eyeD[1], -eyeD[2]);

       glNormal3f(normeyeG[0], normeyeG[1], -normeyeG[2]);
       glVertex3f(eyeG[0], eyeG[1], -eyeG[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeH[0], normeyeH[1], -normeyeH[2]);
       glVertex3f(eyeH[0], eyeH[1], -eyeH[2]);

       glNormal3f(normeyeH[0], normeyeH[1], -normeyeH[2]);
       glVertex3f(eyeH[0], eyeH[1], -eyeH[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeA[0], normeyeA[1], -normeyeA[2]);
       glVertex3f(eyeA[0], eyeA[1], -eyeA[2]);
     glEnd();
*/

     /* Bottom half */
     if((fish[fishnum].blinking == EYE_HALF_OPEN) || (fish[fishnum].blinking == EYE_HALF_CLOSED)){
       glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, eyeamb);
       glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, eyedif);
       glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, eyespec);
       glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, femissive);
       glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
     }
	glCallList(eyeLists+1);
/*     glBegin(GL_TRIANGLES);
       glNormal3f(normeyeD[0], normeyeD[1], -normeyeD[2]);
       glVertex3f(eyeD[0], eyeD[1], -eyeD[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeE[0], normeyeE[1], -normeyeE[2]);
       glVertex3f(eyeE[0], eyeE[1], -eyeE[2]);

       glNormal3f(normeyeE[0], normeyeE[1], -normeyeE[2]);
       glVertex3f(eyeE[0], eyeE[1], -eyeE[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeF[0], normeyeF[1], -normeyeF[2]);
       glVertex3f(eyeF[0], eyeF[1], -eyeF[2]);

       glNormal3f(normeyeF[0], normeyeF[1], -normeyeF[2]);
       glVertex3f(eyeF[0], eyeF[1], -eyeF[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeG[0], normeyeG[1], -normeyeG[2]);
       glVertex3f(eyeG[0], eyeG[1], -eyeG[2]);
     glEnd();
*/
     //glPopMatrix();


     //glPushMatrix();

     glShadeModel(GL_FLAT);
     /* Pupil */
     if(fish[fishnum].blinking != EYE_CLOSED){

       glTranslated(eyeI[0], eyeI[1], -(eyeI[2]+.1));
       glScalef(.2, .2, .2);
       glTranslated(-eyeI[0], -eyeI[1], eyeI[2]);

       glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pupilamb);
       glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pupildif);
       glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pupilspec);
       glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, femissive);
       glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);

       if(fish[fishnum].blinking == EYE_OPEN){

       	glCallList(eyeLists+2);
/*	 glBegin(GL_TRIANGLES);

	 glNormal3f(normeyeA[0], normeyeA[1], -normeyeA[2]);
	 glVertex3f(eyeA[0], eyeA[1], -eyeA[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
	 glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
	 glNormal3f(normeyeB[0], normeyeB[1], -normeyeB[2]);
	 glVertex3f(eyeB[0], eyeB[1], -eyeB[2]);

	 glNormal3f(normeyeB[0], normeyeB[1], -normeyeB[2]);
	 glVertex3f(eyeB[0], eyeB[1], -eyeB[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
	 glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
	 glNormal3f(normeyeC[0], normeyeC[1], -normeyeC[2]);
	 glVertex3f(eyeC[0], eyeC[1], -eyeC[2]);

	 glNormal3f(normeyeC[0], normeyeC[1], -normeyeC[2]);
	 glVertex3f(eyeC[0], eyeC[1], -eyeC[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
	 glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
	 glNormal3f(normeyeD[0], normeyeD[1], -normeyeD[2]);
	 glVertex3f(eyeD[0], eyeD[1], -eyeD[2]);

	 glNormal3f(normeyeG[0], normeyeG[1], -normeyeG[2]);
	 glVertex3f(eyeG[0], eyeG[1], -eyeG[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
	 glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
	 glNormal3f(normeyeH[0], normeyeH[1], -normeyeH[2]);
	 glVertex3f(eyeH[0], eyeH[1], -eyeH[2]);

	 glNormal3f(normeyeH[0], normeyeH[1], -normeyeH[2]);
	 glVertex3f(eyeH[0], eyeH[1], -eyeH[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
	 glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
	 glNormal3f(normeyeA[0], normeyeA[1], -normeyeA[2]);
	 glVertex3f(eyeA[0], eyeA[1], -eyeA[2]);

	 glEnd();
	*/
       }
       glCallList(eyeLists+3);
/*       glBegin(GL_TRIANGLES);

       glNormal3f(normeyeD[0], normeyeD[1], -normeyeD[2]);
       glVertex3f(eyeD[0], eyeD[1], -eyeD[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeE[0], normeyeE[1], -normeyeE[2]);
       glVertex3f(eyeE[0], eyeE[1], -eyeE[2]);

       glNormal3f(normeyeE[0], normeyeE[1], -normeyeE[2]);
       glVertex3f(eyeE[0], eyeE[1], -eyeE[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeF[0], normeyeF[1], -normeyeF[2]);
       glVertex3f(eyeF[0], eyeF[1], -eyeF[2]);

       glNormal3f(normeyeF[0], normeyeF[1], -normeyeF[2]);
       glVertex3f(eyeF[0], eyeF[1], -eyeF[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeG[0], normeyeG[1], -normeyeG[2]);
       glVertex3f(eyeG[0], eyeG[1], -eyeG[2]);
       glEnd();
*/     }
     glShadeModel(GL_SMOOTH);

     glPopMatrix();
   }
   else{
   	glCallList(eyeLists+4);
//     glShadeModel(GL_FLAT);
     /* Dead Eye */
/*     glPushMatrix();
     glTranslated(eyeI[0], eyeI[1], -(eyeI[2]-3));
     glRotated(9.5, 1, 0, 0);
     glRotated(24, 0, -1, 0);
     glTranslated(-eyeI[0], -eyeI[1], (eyeI[2]-3));

     glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pupilamb);
     glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pupildif);
     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pupilspec);
 //    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive);
     glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);

     glBegin(GL_QUADS);
       glVertex3f(deadeyeA[0], deadeyeA[1], -deadeyeA[2]);
       glVertex3f(deadeyeH[0], deadeyeH[1], -deadeyeH[2]);
       glVertex3f(deadeyeE[0], deadeyeE[1], -deadeyeE[2]);
       glVertex3f(deadeyeD[0], deadeyeD[1], -deadeyeD[2]);

       glVertex3f(deadeyeC[0], deadeyeC[1], -deadeyeC[2]);
       glVertex3f(deadeyeB[0], deadeyeB[1], -deadeyeB[2]);
       glVertex3f(deadeyeG[0], deadeyeG[1], -deadeyeG[2]);
       glVertex3f(deadeyeF[0], deadeyeF[1], -deadeyeF[2]);
     glEnd();

     glPopMatrix();
     glShadeModel(GL_SMOOTH);
*/   }
   //glFrontFace(GL_CCW);

	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
  /* Label */
   /*
   glPushMatrix();
   //glRotatef(-(fish[fishnum].rotation+90), 0, 1, 0);
   glEnable(GL_TEXTURE_2D);
   glBegin(GL_QUADS);
     glTexCoord2f(0.0, 0.0); glVertex3f(labelA[0], labelA[1], labelA[2]);
     glTexCoord2f(0.0, 1.0); glVertex3f(labelB[0], labelB[1], labelB[2]);
     glTexCoord2f(1.0, 1.0); glVertex3f(labelC[0], labelC[1], labelC[2]);
     glTexCoord2f(1.0, 0.0); glVertex3f(labelD[0], labelD[1], labelD[2]);
   glEnd();
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
   */


   //glDisable(GL_CULL_FACE);
   //glFlush ();
   glPopName();
   glPopMatrix();
//	stopFishShaders();

}




void ShutDown_fish() {
	glDeleteLists(fishList, 2);
	glDeleteLists(numberList, 128);
	glDeleteLists(tailLists, 6);
	glDeleteLists(eyeLists, 5);

	if(VBOUsed) {
#ifndef WIN32
	glDeleteBuffersARB(1, &VBOVertices);
	glDeleteBuffersARB(1, &VBONormals);
#endif
	}

}


int IsExtensionSupported( char* szTargetExtension )
{
	const unsigned char *pszExtensions = NULL;
	const unsigned char *pszStart;
	unsigned char *pszWhere, *pszTerminator;

	// Extension names should not have spaces
	pszWhere = (unsigned char *) strchr( szTargetExtension, ' ' );
	if( pszWhere || *szTargetExtension == '\0' )
		return false;

	// Get Extensions String
	pszExtensions = glGetString( GL_EXTENSIONS );

	// Search The Extensions String For An Exact Copy
	pszStart = pszExtensions;
	for(;;)
	{
		pszWhere = (unsigned char *) strstr( (const char *) pszStart, szTargetExtension );
		if( !pszWhere )
			break;
		pszTerminator = pszWhere + strlen( szTargetExtension );
		if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
			if( *pszTerminator == ' ' || *pszTerminator == '\0' )
				return true;
		pszStart = pszTerminator;
	}
	return false;
}

void init_fish() {


	createShaders();

	//calculate normals


	find_normal(T, A, S, norm1);
    find_normal(S, A, B, norm2);
    find_normal(S, B, C, norm3);
    find_normal(S, C, D, norm4);
    find_normal(S, D, E, norm5);
    find_normal(S, E, U, norm6);
    find_normal(S, U, T, norm7);
    find_normal(T, U, M, norm8);
    find_normal(T, M, N, norm9);
    find_normal(T, N, O, norm10);
    find_normal(T, O, P, norm11);
    find_normal(T, P, Q, norm12);
    find_normal(T, Q, R, norm13);
    find_normal(G, H, F, norm14);
    find_normal(H, I, F, norm15);
    find_normal(F, I, E, norm16);
    find_normal(E, I, U, norm17);
    find_normal(U, I, M, norm18);
    find_normal(M, I, L, norm19);
    find_normal(L, I, J, norm20);
    find_normal(L, J, K, norm21);

    find_normal(eyeA, eyeI, eyeB, normeye1);
    find_normal(eyeB, eyeI, eyeC, normeye2);
    find_normal(eyeC, eyeI, eyeD, normeye3);
    find_normal(eyeD, eyeI, eyeE, normeye4);
    find_normal(eyeE, eyeI, eyeF, normeye5);
    find_normal(eyeF, eyeI, eyeG, normeye6);
    find_normal(eyeG, eyeI, eyeH, normeye7);
    find_normal(eyeH, eyeI, eyeA, normeye8);

    normeyeA[0] = (normeye8[0] + normeye1[0])/2;
    normeyeA[1] = (normeye8[1] + normeye1[1])/2;
    normeyeA[2] = (normeye8[2] + normeye1[2])/2;

    normeyeB[0] = (normeye1[0] + normeye2[0])/2;
    normeyeB[1] = (normeye1[1] + normeye2[1])/2;
    normeyeB[2] = (normeye1[2] + normeye2[2])/2;

    normeyeC[0] = (normeye2[0] + normeye3[0])/2;
    normeyeC[1] = (normeye2[1] + normeye3[1])/2;
    normeyeC[2] = (normeye2[2] + normeye3[2])/2;

    normeyeD[0] = (normeye3[0] + normeye4[0])/2;
    normeyeD[1] = (normeye3[1] + normeye4[1])/2;
    normeyeD[2] = (normeye3[2] + normeye4[2])/2;

    normeyeE[0] = (normeye4[0] + normeye5[0])/2;
    normeyeE[1] = (normeye4[1] + normeye5[1])/2;
    normeyeE[2] = (normeye4[2] + normeye5[2])/2;

    normeyeF[0] = (normeye5[0] + normeye6[0])/2;
    normeyeF[1] = (normeye5[1] + normeye6[1])/2;
    normeyeF[2] = (normeye5[2] + normeye6[2])/2;

    normeyeG[0] = (normeye6[0] + normeye7[0])/2;
    normeyeG[1] = (normeye6[1] + normeye7[1])/2;
    normeyeG[2] = (normeye6[2] + normeye7[2])/2;

    normeyeH[0] = (normeye7[0] + normeye8[0])/2;
    normeyeH[1] = (normeye7[1] + normeye8[1])/2;
    normeyeH[2] = (normeye7[2] + normeye8[2])/2;

    normeyeI[0] = (normeye1[0] + normeye2[0] + normeye3[0] + normeye4[0] + normeye5[0] + normeye6[0] + normeye7[0] + normeye8[0])/8;
    normeyeI[1] = (normeye1[1] + normeye2[1] + normeye3[1] + normeye4[1] + normeye5[1] + normeye6[1] + normeye7[1] + normeye8[1])/8;
    normeyeI[2] = (normeye1[2] + normeye2[2] + normeye3[2] + normeye4[2] + normeye5[2] + normeye6[2] + normeye7[2] + normeye8[2])/8;



    normA[0] = (norm1[0] + norm2[0])/2;
    normA[1] = (norm1[1] + norm2[1])/2;
    normA[2] = (norm1[2] + norm2[2])/2;

    normB[0] = (norm2[0] + norm2[0])/2;
    normB[1] = (norm2[1] + norm2[1])/2;
    normB[2] = (norm2[2] + norm2[2])/2;

    normC[0] = (norm3[0] + norm4[0])/2;
    normC[1] = (norm3[1] + norm4[1])/2;
    normC[2] = (norm3[2] + norm4[2])/2;

    normD[0] = (norm4[0] + norm5[0])/2;
    normD[1] = (norm4[1] + norm5[1])/2;
    normD[2] = (norm4[2] + norm5[2])/2;

    normE[0] = (norm5[0] + norm6[0] + norm17[0] + norm16[0])/4;
    normE[1] = (norm5[1] + norm6[1] + norm17[1] + norm16[1])/4;
    normE[2] = (norm5[2] + norm6[2] + norm17[2] + norm16[2])/4;

    normF[0] = (norm16[0] + norm15[0] + norm14[0])/3;
    normF[1] = (norm16[1] + norm15[1] + norm14[1])/3;
    normF[2] = (norm16[2] + norm15[2] + norm14[2])/3;

    normG[0] = norm14[0];
    normG[1] = norm14[1];
    normG[2] = norm14[2];

    normH[0] = (norm14[0] + norm15[0])/2;
    normH[1] = (norm14[1] + norm15[1])/2;
    normH[2] = (norm14[2] + norm15[2])/2;

    normI[0] = (norm15[0] + norm16[0] + norm17[0] + norm18[0] + norm19[0] + norm20[0])/6;
    normI[1] = (norm15[1] + norm16[1] + norm17[1] + norm18[1] + norm19[1] + norm20[1])/6;
    normI[2] = (norm15[2] + norm16[2] + norm17[2] + norm18[2] + norm19[2] + norm20[2])/6;

    normJ[0] = (norm20[0] + norm21[0])/2;
    normJ[1] = (norm20[1] + norm21[1])/2;
    normJ[2] = (norm20[2] + norm21[2])/2;

    normK[0] = norm21[0];
    normK[1] = norm21[1];
    normK[2] = norm21[2];

    normL[0] = (norm21[0] + norm20[0] + norm19[0])/3;
    normL[1] = (norm21[1] + norm20[1] + norm19[1])/3;
    normL[2] = (norm21[2] + norm20[2] + norm19[2])/3;

    normM[0] = (norm19[0] + norm18[0] + norm8[0] + norm9[0])/4;
    normM[1] = (norm19[1] + norm18[1] + norm8[1] + norm9[1])/4;
    normM[2] = (norm19[2] + norm18[2] + norm8[2] + norm9[2])/4;

    normN[0] = (norm9[0] + norm10[0])/2;
    normN[1] = (norm9[1] + norm10[1])/2;
    normN[2] = (norm9[2] + norm10[2])/2;

    normO[0] = (norm10[0] + norm11[0])/2;
    normO[1] = (norm10[1] + norm11[1])/2;
    normO[2] = (norm10[2] + norm11[2])/2;

    normP[0] = (norm11[0] + norm12[0])/2;
    normP[1] = (norm11[1] + norm12[1])/2;
    normP[2] = (norm11[2] + norm12[2])/2;

    normQ[0] = (norm12[0] + norm13[0])/2;
    normQ[1] = (norm12[1] + norm13[1])/2;
    normQ[2] = (norm12[2] + norm13[2])/2;

    normR[0] = norm13[0];
    normR[1] = norm13[1];
    normR[2] = norm13[2];

    normS[0] = (norm1[0] + norm2[0] + norm3[0] + norm4[0] + norm5[0] + norm6[0] + norm7[0])/7;
    normS[1] = (norm1[1] + norm2[1] + norm3[1] + norm4[1] + norm5[1] + norm6[1] + norm7[1])/7;
    normS[2] = (norm1[2] + norm2[2] + norm3[2] + norm4[2] + norm5[2] + norm6[2] + norm7[2])/7;

    normT[0] = (norm1[0] + norm7[0] + norm8[0] + norm9[0] + norm10[0] + norm11[0] + norm12[0] + norm13[0])/8;
    normT[1] = (norm1[1] + norm7[1] + norm8[1] + norm9[1] + norm10[1] + norm11[1] + norm12[1] + norm13[1])/8;
    normT[2] = (norm1[2] + norm7[2] + norm8[2] + norm9[2] + norm10[2] + norm11[2] + norm12[2] + norm13[2])/8;

    normU[0] = (norm6[0] + norm7[0] + norm8[0] + norm18[0] + norm19[0])/5;
    normU[1] = (norm6[1] + norm7[1] + norm8[1] + norm18[1] + norm19[1])/5;
    normU[2] = (norm6[2] + norm7[2] + norm8[2] + norm18[2] + norm19[2])/5;













	tailLists = glGenLists(6);
	glNewList(tailLists, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	   glTranslated(-E[0], -E[1], -E[2]);
     glNormal3f(normF[0], normF[1], normF[2]);
     glVertex3f(F[0], F[1], F[2]);
     glNormal3f(normI[0], normI[1], normI[2]);
     glVertex3f(I[0], I[1], I[2]);
     glNormal3f(normE[0], normE[1], normE[2]);
     glVertex3f(E[0], E[1], E[2]);
     glEnd();

        glPushMatrix();
   glTranslated(F[0], F[1], F[2]);
     glEndList();


     glNewList(tailLists+1, GL_COMPILE);
        glTranslated(-F[0], -F[1], -F[2]);

   glBegin(GL_TRIANGLES);
     glNormal3f(normH[0], normH[1], normH[2]);
     glVertex3f(H[0], H[1], H[2]);
     glNormal3f(normI[0], normI[1], normI[2]);
     glVertex3f(I[0], I[1], I[2]);
     glNormal3f(normF[0], normF[1], normF[2]);
     glVertex3f(F[0], F[1], F[2]);
   glEnd();

      glPushMatrix();
   glTranslated(F[0], F[1], F[2]);

     glEndList();


         glNewList(tailLists+2, GL_COMPILE);
      glTranslated(-F[0], -F[1], -F[2]);

   /* 14 */
   glBegin(GL_TRIANGLES);
     glNormal3f(normG[0], normG[1], normG[2]);
     glVertex3f(G[0], G[1], G[2]);
     glNormal3f(normH[0], normH[1], normH[2]);
     glVertex3f(H[0], H[1], H[2]);
     glNormal3f(normF[0], normF[1], normF[2]);
     glVertex3f(F[0], F[1], F[2]);
   glEnd();

   glPopMatrix();
   glPopMatrix();
   glPopMatrix();

     glEndList();

              glNewList(tailLists+3, GL_COMPILE);
     glTranslated(-M[0], -M[1], -M[2]);

   glBegin(GL_TRIANGLES);
     glNormal3f(normM[0], normM[1], normM[2]);
     glVertex3f(M[0], M[1], M[2]);
     glNormal3f(normI[0], normI[1], normI[2]);
     glVertex3f(I[0], I[1], I[2]);
     glNormal3f(normL[0], normL[1], normL[2]);
     glVertex3f(L[0], L[1], L[2]);
   glEnd();

   glPushMatrix();
   glTranslated(L[0], L[1], L[2]);

     glEndList();


                   glNewList(tailLists+4, GL_COMPILE);
     glTranslated(-L[0], -L[1], -L[2]);

   glBegin(GL_TRIANGLES);
     glNormal3f(normL[0], normL[1], normL[2]);
     glVertex3f(L[0], L[1], L[2]);
     glNormal3f(normI[0], normI[1], normI[2]);
     glVertex3f(I[0], I[1], I[2]);
     glNormal3f(normK[0], normK[1], normK[2]);
     glVertex3f(J[0], J[1], J[2]);
   glEnd();

   glPushMatrix();
   glTranslated(L[0], L[1], L[2]);

     glEndList();



                   glNewList(tailLists+5, GL_COMPILE);
      glTranslated(-L[0], -L[1], -L[2]);

   glBegin(GL_TRIANGLES);
     glNormal3f(normL[0], normL[1], normL[2]);
     glVertex3f(L[0], L[1], L[2]);
     glNormal3f(normJ[0], normJ[1], normJ[2]);
     glVertex3f(J[0], J[1], J[2]);
     glNormal3f(normK[0], normK[1], normK[2]);
     glVertex3f(K[0], K[1], K[2]);
   glEnd();

   glPopMatrix();
   glPopMatrix();
   glPopMatrix();


   glPopMatrix();


   glPopMatrix();

     glEndList();



	eyeLists = glGenLists(10);


	glNewList(eyeLists, GL_COMPILE);
	 glTranslated(eyeI[0], eyeI[1], -eyeI[2]);
     glRotated(5, 1, 0, 0);
     glRotated(29, 0, -1, 0);
     glTranslated(-eyeI[0], -eyeI[1], (eyeI[2]));


     glBegin(GL_TRIANGLES);

       glNormal3f(normeyeA[0], normeyeA[1], -normeyeA[2]);
       glVertex3f(eyeA[0], eyeA[1], -eyeA[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeB[0], normeyeB[1], -normeyeB[2]);
       glVertex3f(eyeB[0], eyeB[1], -eyeB[2]);

       glNormal3f(normeyeB[0], normeyeB[1], -normeyeB[2]);
       glVertex3f(eyeB[0], eyeB[1], -eyeB[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeC[0], normeyeC[1], -normeyeC[2]);
       glVertex3f(eyeC[0], eyeC[1], -eyeC[2]);

       glNormal3f(normeyeC[0], normeyeC[1], -normeyeC[2]);
       glVertex3f(eyeC[0], eyeC[1], -eyeC[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeD[0], normeyeD[1], -normeyeD[2]);
       glVertex3f(eyeD[0], eyeD[1], -eyeD[2]);

       glNormal3f(normeyeG[0], normeyeG[1], -normeyeG[2]);
       glVertex3f(eyeG[0], eyeG[1], -eyeG[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeH[0], normeyeH[1], -normeyeH[2]);
       glVertex3f(eyeH[0], eyeH[1], -eyeH[2]);

       glNormal3f(normeyeH[0], normeyeH[1], -normeyeH[2]);
       glVertex3f(eyeH[0], eyeH[1], -eyeH[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeA[0], normeyeA[1], -normeyeA[2]);
       glVertex3f(eyeA[0], eyeA[1], -eyeA[2]);
     glEnd();

	glEndList();




	glNewList(eyeLists+1, GL_COMPILE);
	    glBegin(GL_TRIANGLES);
       glNormal3f(normeyeD[0], normeyeD[1], -normeyeD[2]);
       glVertex3f(eyeD[0], eyeD[1], -eyeD[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeE[0], normeyeE[1], -normeyeE[2]);
       glVertex3f(eyeE[0], eyeE[1], -eyeE[2]);

       glNormal3f(normeyeE[0], normeyeE[1], -normeyeE[2]);
       glVertex3f(eyeE[0], eyeE[1], -eyeE[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeF[0], normeyeF[1], -normeyeF[2]);
       glVertex3f(eyeF[0], eyeF[1], -eyeF[2]);

       glNormal3f(normeyeF[0], normeyeF[1], -normeyeF[2]);
       glVertex3f(eyeF[0], eyeF[1], -eyeF[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeG[0], normeyeG[1], -normeyeG[2]);
       glVertex3f(eyeG[0], eyeG[1], -eyeG[2]);
     glEnd();

	glEndList();


	glNewList(eyeLists+2, GL_COMPILE);
	glBegin(GL_TRIANGLES);

	 glNormal3f(normeyeA[0], normeyeA[1], -normeyeA[2]);
	 glVertex3f(eyeA[0], eyeA[1], -eyeA[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
	 glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
	 glNormal3f(normeyeB[0], normeyeB[1], -normeyeB[2]);
	 glVertex3f(eyeB[0], eyeB[1], -eyeB[2]);

	 glNormal3f(normeyeB[0], normeyeB[1], -normeyeB[2]);
	 glVertex3f(eyeB[0], eyeB[1], -eyeB[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
	 glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
	 glNormal3f(normeyeC[0], normeyeC[1], -normeyeC[2]);
	 glVertex3f(eyeC[0], eyeC[1], -eyeC[2]);

	 glNormal3f(normeyeC[0], normeyeC[1], -normeyeC[2]);
	 glVertex3f(eyeC[0], eyeC[1], -eyeC[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
	 glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
	 glNormal3f(normeyeD[0], normeyeD[1], -normeyeD[2]);
	 glVertex3f(eyeD[0], eyeD[1], -eyeD[2]);

	 glNormal3f(normeyeG[0], normeyeG[1], -normeyeG[2]);
	 glVertex3f(eyeG[0], eyeG[1], -eyeG[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
	 glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
	 glNormal3f(normeyeH[0], normeyeH[1], -normeyeH[2]);
	 glVertex3f(eyeH[0], eyeH[1], -eyeH[2]);

	 glNormal3f(normeyeH[0], normeyeH[1], -normeyeH[2]);
	 glVertex3f(eyeH[0], eyeH[1], -eyeH[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
	 glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
	 glNormal3f(normeyeA[0], normeyeA[1], -normeyeA[2]);
	 glVertex3f(eyeA[0], eyeA[1], -eyeA[2]);

	 glEnd();

	glEndList();



	glNewList(eyeLists+3, GL_COMPILE);
	glBegin(GL_TRIANGLES);

       glNormal3f(normeyeD[0], normeyeD[1], -normeyeD[2]);
       glVertex3f(eyeD[0], eyeD[1], -eyeD[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeE[0], normeyeE[1], -normeyeE[2]);
       glVertex3f(eyeE[0], eyeE[1], -eyeE[2]);

       glNormal3f(normeyeE[0], normeyeE[1], -normeyeE[2]);
       glVertex3f(eyeE[0], eyeE[1], -eyeE[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeF[0], normeyeF[1], -normeyeF[2]);
       glVertex3f(eyeF[0], eyeF[1], -eyeF[2]);

       glNormal3f(normeyeF[0], normeyeF[1], -normeyeF[2]);
       glVertex3f(eyeF[0], eyeF[1], -eyeF[2]);
       glNormal3f(normeyeI[0], normeyeI[1], -normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], -eyeI[2]);
       glNormal3f(normeyeG[0], normeyeG[1], -normeyeG[2]);
       glVertex3f(eyeG[0], eyeG[1], -eyeG[2]);
       glEnd();

	glEndList();


	glNewList(eyeLists+4, GL_COMPILE);
	glShadeModel(GL_FLAT);
     /* Dead Eye */
     glPushMatrix();
     glTranslated(eyeI[0], eyeI[1], -(eyeI[2]-3));
     glRotated(9.5, 1, 0, 0);
     glRotated(24, 0, -1, 0);
     glTranslated(-eyeI[0], -eyeI[1], (eyeI[2]-3));

     glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pupilamb);
     glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pupildif);
     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pupilspec);
     glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive);
     glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);

     glBegin(GL_QUADS);
       glVertex3f(deadeyeA[0], deadeyeA[1], -deadeyeA[2]);
       glVertex3f(deadeyeH[0], deadeyeH[1], -deadeyeH[2]);
       glVertex3f(deadeyeE[0], deadeyeE[1], -deadeyeE[2]);
       glVertex3f(deadeyeD[0], deadeyeD[1], -deadeyeD[2]);

       glVertex3f(deadeyeC[0], deadeyeC[1], -deadeyeC[2]);
       glVertex3f(deadeyeB[0], deadeyeB[1], -deadeyeB[2]);
       glVertex3f(deadeyeG[0], deadeyeG[1], -deadeyeG[2]);
       glVertex3f(deadeyeF[0], deadeyeF[1], -deadeyeF[2]);
     glEnd();

     glPopMatrix();
     glShadeModel(GL_SMOOTH);

	glEndList();

	glNewList(eyeLists+5, GL_COMPILE);


	     glTranslated(eyeI[0], eyeI[1], eyeI[2]);
     glRotated(5, -1, 0, 0);
     glRotated(29, 0, 1, 0);
     glTranslated(-eyeI[0], -eyeI[1], -(eyeI[2]));

     glBegin(GL_TRIANGLES);

       glNormal3f(normeyeA[0], normeyeA[1], normeyeA[2]);
       glVertex3f(eyeA[0], eyeA[1], eyeA[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeB[0], normeyeB[1], normeyeB[2]);
       glVertex3f(eyeB[0], eyeB[1], eyeB[2]);

       glNormal3f(normeyeB[0], normeyeB[1], normeyeB[2]);
       glVertex3f(eyeB[0], eyeB[1], eyeB[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeC[0], normeyeC[1], normeyeC[2]);
       glVertex3f(eyeC[0], eyeC[1], eyeC[2]);

       glNormal3f(normeyeC[0], normeyeC[1], normeyeC[2]);
       glVertex3f(eyeC[0], eyeC[1], eyeC[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeD[0], normeyeD[1], normeyeD[2]);
       glVertex3f(eyeD[0], eyeD[1], eyeD[2]);

       glNormal3f(normeyeG[0], normeyeG[1], normeyeG[2]);
       glVertex3f(eyeG[0], eyeG[1], eyeG[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeH[0], normeyeH[1], normeyeH[2]);
       glVertex3f(eyeH[0], eyeH[1], eyeH[2]);

       glNormal3f(normeyeH[0], normeyeH[1], normeyeH[2]);
       glVertex3f(eyeH[0], eyeH[1], eyeH[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeA[0], normeyeA[1], normeyeA[2]);
       glVertex3f(eyeA[0], eyeA[1], eyeA[2]);

     glEnd();







	glEndList();

		glNewList(eyeLists+6, GL_COMPILE);



	     glBegin(GL_TRIANGLES);

       glNormal3f(normeyeD[0], normeyeD[1], normeyeD[2]);
       glVertex3f(eyeD[0], eyeD[1], eyeD[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeE[0], normeyeE[1], normeyeE[2]);
       glVertex3f(eyeE[0], eyeE[1], eyeE[2]);

       glNormal3f(normeyeE[0], normeyeE[1], normeyeE[2]);
       glVertex3f(eyeE[0], eyeE[1], eyeE[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeF[0], normeyeF[1], normeyeF[2]);
       glVertex3f(eyeF[0], eyeF[1], eyeF[2]);

       glNormal3f(normeyeF[0], normeyeF[1], normeyeF[2]);
       glVertex3f(eyeF[0], eyeF[1], eyeF[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeG[0], normeyeG[1], normeyeG[2]);
       glVertex3f(eyeG[0], eyeG[1], eyeG[2]);

     glEnd();

     //glPopMatrix();

     //glPushMatrix();

     glShadeModel(GL_FLAT);






	glEndList();



		glNewList(eyeLists+7, GL_COMPILE);


	glBegin(GL_TRIANGLES);

	 glNormal3f(normeyeA[0], normeyeA[1], normeyeA[2]);
	 glVertex3f(eyeA[0], eyeA[1], eyeA[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]+1);
	 glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
	 glNormal3f(normeyeB[0], normeyeB[1], normeyeB[2]);
	 glVertex3f(eyeB[0], eyeB[1], eyeB[2]);


	 glNormal3f(normeyeB[0], normeyeB[1], normeyeB[2]);
	 glVertex3f(eyeB[0], eyeB[1], eyeB[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]+1);
	 glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
	 glNormal3f(normeyeC[0], normeyeC[1], normeyeC[2]);
	 glVertex3f(eyeC[0], eyeC[1], eyeC[2]);

	 glNormal3f(normeyeC[0], normeyeC[1], normeyeC[2]);
	 glVertex3f(eyeC[0], eyeC[1], eyeC[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]+1);
	 glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
	 glNormal3f(normeyeD[0], normeyeD[1], normeyeD[2]);
	 glVertex3f(eyeD[0], eyeD[1], eyeD[2]);

	 glNormal3f(normeyeG[0], normeyeG[1], normeyeG[2]);
	 glVertex3f(eyeG[0], eyeG[1], eyeG[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]+1);
	 glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
	 glNormal3f(normeyeH[0], normeyeH[1], normeyeH[2]);
	 glVertex3f(eyeH[0], eyeH[1], eyeH[2]);

	 glNormal3f(normeyeH[0], normeyeH[1], normeyeH[2]);
	 glVertex3f(eyeH[0], eyeH[1], eyeH[2]);
	 glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]+1);
	 glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
	 glNormal3f(normeyeA[0], normeyeA[1], normeyeA[2]);
	 glVertex3f(eyeA[0], eyeA[1], eyeA[2]);

	 glEnd();







	glEndList();



		glNewList(eyeLists+8, GL_COMPILE);



	      glBegin(GL_TRIANGLES);

       glNormal3f(normeyeD[0], normeyeD[1], normeyeD[2]);
       glVertex3f(eyeD[0], eyeD[1], eyeD[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]+1);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeE[0], normeyeE[1], normeyeE[2]);
       glVertex3f(eyeE[0], eyeE[1], eyeE[2]);

       glNormal3f(normeyeE[0], normeyeE[1], normeyeE[2]);
       glVertex3f(eyeE[0], eyeE[1], eyeE[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]+1);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeF[0], normeyeF[1], normeyeF[2]);
       glVertex3f(eyeF[0], eyeF[1], eyeF[2]);

       glNormal3f(normeyeF[0], normeyeF[1], normeyeF[2]);
       glVertex3f(eyeF[0], eyeF[1], eyeF[2]);
       glNormal3f(normeyeI[0], normeyeI[1], normeyeI[2]+1);
       glVertex3f(eyeI[0], eyeI[1], eyeI[2]);
       glNormal3f(normeyeG[0], normeyeG[1], normeyeG[2]);
       glVertex3f(eyeG[0], eyeG[1], eyeG[2]);

       glEnd();






	glEndList();



		glNewList(eyeLists+9, GL_COMPILE);


	glShadeModel(GL_FLAT);
     glPushMatrix();
     glTranslated(eyeI[0], eyeI[1], eyeI[2]-3);
     glRotated(9.5, -1, 0, 0);
     glRotated(24, 0, 1, 0);
     glTranslated(-eyeI[0], -eyeI[1], -(eyeI[2]-3));

     glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pupilamb);
     glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pupildif);
     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pupilspec);
     glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive);
     glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);

     glBegin(GL_QUADS);
       glVertex3f(deadeyeA[0], deadeyeA[1], deadeyeA[2]);
       glVertex3f(deadeyeH[0], deadeyeH[1], deadeyeH[2]);
       glVertex3f(deadeyeE[0], deadeyeE[1], deadeyeE[2]);
       glVertex3f(deadeyeD[0], deadeyeD[1], deadeyeD[2]);

       glVertex3f(deadeyeC[0], deadeyeC[1], deadeyeC[2]);
       glVertex3f(deadeyeB[0], deadeyeB[1], deadeyeB[2]);
       glVertex3f(deadeyeG[0], deadeyeG[1], deadeyeG[2]);
       glVertex3f(deadeyeF[0], deadeyeF[1], deadeyeF[2]);
     glEnd();
     glPopMatrix();
     glShadeModel(GL_SMOOTH);







	glEndList();




	static float rightVertArray[117] = {	T[0], T[1], T[2],
									A[0], A[1], A[2],
									S[0], S[1], S[2],

									S[0], S[1], S[2],
									A[0], A[1], A[2],
									B[0], B[1], B[2],

									S[0], S[1], S[2],
									B[0], B[1], B[2],
									C[0], C[1], C[2],

									S[0], S[1], S[2],
									C[0], C[1], C[2],
									D[0], D[1], D[2],

									S[0], S[1], S[2],
									D[0], D[1], D[2],
									E[0], E[1], E[2],

									S[0], S[1], S[2],
									E[0], E[1], E[2],
									U[0], U[1], U[2],

									S[0], S[1], S[2],
									U[0], U[1], U[2],
									T[0], T[1], T[2],

									T[0], T[1], T[2],
									U[0], U[1], U[2],
									M[0], M[1], M[2],

									T[0], T[1], T[2],
									M[0], M[1], M[2],
									N[0], N[1], N[2],

									T[0], T[1], T[2],
									N[0], N[1], N[2],
									O[0], O[1], O[2],

									T[0], T[1], T[2],
									O[0], O[1], O[2],
									P[0], P[1], P[2],

									T[0], T[1], T[2],
									P[0], P[1], P[2],
									Q[0], Q[1], Q[2],

									T[0], T[1], T[2],
									Q[0], Q[1], Q[2],
									R[0], R[1], R[2] };



	static float rightNormArray[117] = {	normT[0], normT[1], normT[2],
									normA[0], normA[1], normA[2],
									normS[0], normS[1], normS[2],

									normS[0], normS[1], normS[2],
									normA[0], normA[1], normA[2],
									normB[0], normB[1], normB[2],

									normS[0], normS[1], normS[2],
									normB[0], normB[1], normB[2],
									normC[0], normC[1], normC[2],

									normS[0], normS[1], normS[2],
									normC[0], normC[1], normC[2],
									normD[0], normD[1], normD[2],

									normS[0], normS[1], normS[2],
									normD[0], normD[1], normD[2],
									normE[0], normE[1], normE[2],

									normS[0], normS[1], normS[2],
									normE[0], normE[1], normE[2],
									normU[0], normU[1], normU[2],

									normS[0], normS[1], normS[2],
									normU[0], normU[1], normU[2],
									normT[0], normT[1], normT[2],

									normT[0], normT[1], normT[2],
									normU[0], normU[1], normU[2],
									normM[0], normM[1], normM[2],

									normT[0], normT[1], normT[2],
									normM[0], normM[1], normM[2],
									normN[0], normN[1], normN[2],

									normT[0], normT[1], normT[2],
									normN[0], normN[1], normN[2],
									normO[0], normO[1], normO[2],

									normT[0], normT[1], normT[2],
									normO[0], normO[1], normO[2],
									normP[0], normP[1], normP[2],

									normT[0], normT[1], normT[2],
									normP[0], normP[1], normP[2],
									normQ[0], normQ[1], normQ[2],

									normT[0], normT[1], normT[2],
									normQ[0], normQ[1], normQ[2],
									normR[0], normR[1], normR[2] };




	//build VBO's
	VBOSupported = IsExtensionSupported((char*)"GL_ARB_vertex_buffer_object");
	//if(IsExtensionSupported("GL_ARB_fragment_shader"))
	//	printf("fragment good\n");
	//if(IsExtensionSupported("GL_ARB_vertex_shader"))
	//	printf("vertex good\n");
	VBOUsed = false;
	/*if(VBOSupported) {
			// Get Pointers To The GL Functions
			printf("VBO is Supported\n");
			GLubyte* name = (GLubyte*)"glGenBuffersARB";
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) glXGetProcAddressARB(name);
			name = (GLubyte*)"glBindBufferARB";
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC) glXGetProcAddressARB(name);
			name = (GLubyte*)"glBufferDataARB";
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC) glXGetProcAddressARB(name);
			name = (GLubyte*)"glDeleteBuffersARB";
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) glXGetProcAddressARB(name);

		glGenBuffersARB( 1,  &VBOVertices);					// Get A Valid Name
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, VBOVertices );			// Bind The Buffer
	// Load The Data
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, 39*3*sizeof(float), rightVertArray, GL_STATIC_DRAW_ARB );

	// Generate And Bind The Normal for right Coordinate Buffer
	glGenBuffersARB( 1, &VBONormals );					// Get A Valid Name
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, VBONormals );		// Bind The Buffer
	// Load The Data
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, 39*3*sizeof(float), rightNormArray, GL_STATIC_DRAW_ARB );

	/*int a = 0;
	for(a = 0; a < 117; a++) {
		if((a+1)%3==0) {
			rightNormArray[a] *= -1.0;
		}
	}

	glGenBuffersARB( 1, &VBONormalsLeft );					// Get A Valid Name
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, VBONormalsLeft );		// Bind The Buffer
	// Load The Data
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, 39*3*sizeof(float), rightNormArray, GL_STATIC_DRAW_ARB );
	*/




	//}





}
