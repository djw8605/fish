

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
 * This File is the main for the aquarium program
 * Authors: Cory Lueninghoener, Derek Weitzel
 *
 */

#ifdef WIN32

#define	WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#elif __APPLE__
#include <sys/time.h>
#include <unistd.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <sys/time.h>
#include <GL/glut.h>
#include <unistd.h>
#include <GL/glx.h>
#endif	//WIN32


#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>



#include "fishmodel.h"
#include "aquarium.h"
#include "castle/castlemodel.h"
#include "collision.h"
#include "collisionMap.h"
#include "fishStats.h"
#include "ErrorDisplay.h"
#include "FishSplash.h"
#include "shaders.h"
#include "LabelDisplay.h"
//currently don't have working
#ifndef WIN32
#include "FreeType.h"


#endif

#include "com/comClass.h"





//#define COLLISIONDEBUG



//time between refreshing status
#define FILEREFRESH 60
//number of bubbles
#define BUBBLECOUNT 40
//average speed of bubbles
//notice, there is random involved
#define BUBBLESPEED 40
//speed that the fish grow
#define GROWTHFACTOR .1
//target or desired frame rate
#define FPSTARGET 25
//speed that the blink occurs
#define BLINKSPEED 30/.5	//30 = 1 sec to blink
//speed the all the fishies fins go
#define FINSPEED 50
//speed the fish swim in their circles
//note that this is an average, random affects this
//mostly effects darting
//too large and fish will begin gathering at sides
#define FISHSPEED 100

//duration of a dart action
//25 is default, also, too large and will become stuck on the sides
#define DARTDURATION 10

//speed at which fish will rotate, going in circles.
//less = fish don't turn at all
#define ROTATIONSPEED .5

//effect night has on the finspeed, and normal speed, should slow almost everything
#define NIGHTFACTOR .1

//used as the time for the camera to move back
#define CAMERATIMEOUT 12

// Minimum speed before it reset the position of the fish
#define MINSPEED 0.2
#define _MINSPEED MINSPEED*MINSPEED




#define PI 3.14159265

#ifndef WIN32
freetype::font_data font;
#endif

struct Bubble
{
    double location[3];
    double speed;
};

double degrad = PI/180.0;

//used in the communication of this program
comClass com;
//used in the error reporting
ErrorDisplay errorMess;

//int xmin = -1333, xmax = 1333;
//int ymin = -500,  ymax = 500;
//int zmin = -500,  zmax = 500;
int winx = 1024;
int winy = 700;

double fov = 36.5;
//wider for wide screen
//double fov = 45;
int SleepTime = 5000;

//used to hold window number
int window;
//used as display list for sides
GLuint wallList = 0;

//used as display list for bubbles
GLuint bubbleList = 0;

//used to see if it should display the frames per second
bool PrintFrames = true;

//used to hold information about individual clusters
ClusterInfo* clusters;

//used to hold the number of clusters being read
int numClusters;

//used to hold the number of fish in the fish array
int FISHCOUNT;

//used to determine if picking is active
bool picking;

//used for showing stats on fish
fishStats fishstats;

//used to hold stats data
Stats stats;

//the amount of stat fields
int statFields;

//list for sand
GLuint sandList;

//used for camera counter
int cameraCounter;

//used for camera constants
int cameraSelected;

//used mostle in camera rotations
bool renderWalls = true;

//used to turn it to dark
bool nightTime = false;

//used for the splash image during startup
FishSplash* fishSplash;

//used for the collision, collision map
collisionMap* cm;

#ifdef WIN32
//for windows use
HWND hWnd;
#endif

extern GLfloat vertBridgeArray[];
extern GLfloat texBridgeArray[];
/*
int xmin = -600, xmax = 600;
int ymin = -600,  ymax = 600;
int zmin = -600,  zmax = 600;
int winx = 500;
int winy = 500;
double fov = 45;
*/


Fish* fish;
struct Bubble bubbles[BUBBLECOUNT];
double bubblerloc[3];

float transparency = 1;

int frames=0;

int surfaceangles[5][5];


#define	checkImageWidth 64
#define	checkImageHeight 64
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];
static GLuint texName;
int night = 0;

void makeCheckImage(void)
{
   int i, j, c;

   for (i = 0; i < checkImageHeight; i++) {
      for (j = 0; j < checkImageWidth; j++) {
         c = ((((i&0x8)==0)^((j&0x8))==0))*255;
         checkImage[i][j][0] = (GLubyte) c;
         checkImage[i][j][1] = (GLubyte) c;
         checkImage[i][j][2] = (GLubyte) c;
         checkImage[i][j][3] = (GLubyte) 255;
      }
   }
}

double storeTime = 0;
double lastTime = 0;
//to attempt to time dependent rendering
//returns the time since last iteration, or frame
double getTime() {
	return storeTime-lastTime;
}
//attempt time dependent

void updateTime() {

	lastTime = storeTime;
#ifndef WIN32
	timeval t;
	gettimeofday(&t, 0);
	//store seconds since epoch
	storeTime = t.tv_sec;
	//store micoseconds of that second, so divide by million
	storeTime += ((double)t.tv_usec / 1000000);
#else
	time_t t;
	time(&t);
	storeTime = t;
	SYSTEMTIME st;
	GetSystemTime(&st);
	storeTime += ((double)st.wMilliseconds/1000);
#endif


}

//Outputs the fps to the console, also optimizes the sleep timer
// to target the value int FPSTARGET
void alarm_handler(int num)
{
	//output the frames per second
	if(PrintFrames) {
  printf("\b\b\b\b\b\b\b\b%3.1f fps", (float)frames);
  fflush(stdout);
	}
  //calculate sleep timer to target the fps target
  double temp = (FPSTARGET - (double)frames) / FPSTARGET;
  SleepTime += (-1)*(int)(((double)SleepTime * temp));
  //end calculate
  frames = 0;
#ifndef WIN32
  alarm(1);
#endif
  cameraCounter++;
}

#ifdef WIN32

DWORD WINAPI AlarmThread(LPVOID ptrVoid) {

	Sleep(1000);
	alarm_handler(0);
	return NULL;
}
#endif


/*  Initialize material property, light source, lighting model,
 *  and depth buffer.
 */
void init(void)
{
  int i,j;
  GLfloat position[] = { 0.0, ymax*2, 0.0, 1.0 };
  GLfloat spotpos[] = { 0, 0, 0, 1 };
  GLfloat spotdir[] = { -1, 0, 0 };
#ifndef WIN32
  try{
  font.init("Test.ttf", 16);
  } catch (std::exception &e) {
  	printf("%s\n", e.what());
  }
#endif
  /*
  GLfloat ambient[] = { 0.3, 0.3, 0.3, 1.0 };
  GLfloat diffuse[] = { .01, .01, .01, 1.0 };
  GLfloat fog[] = { .5, .5, .5, 1.0 };
  */

  GLfloat ambient[] = { 0.5, 0.5, 0.5, 1.0 };
  GLfloat diffuse[] = { 1, 1, 1, 1.0 };
  GLfloat fog[] = { .0, .0, .2, 1.0 };

  GLfloat spotamb[] = { 0.1, 0.1, 0.1, 1.0 };
  GLfloat spotdif[] = { 1, 1, 1, 1.0 };



  glClearColor (0.0, 0.0, 0.0, 0.0);
  //glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  //glEnable(GL_LIGHT1);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glEnable(GL_AUTO_NORMAL);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glBlendFunc (GL_SRC_ALPHA, GL_ZERO);

  glEnable(GL_FOG);
  glFogi(GL_FOG_MODE, GL_EXP2);
  glFogfv(GL_FOG_COLOR, fog);
  glFogf(GL_FOG_DENSITY, .0002);
  glHint(GL_FOG_HINT, GL_DONT_CARE);
  glFogf(GL_FOG_START, 1500.0);
  glFogf(GL_FOG_END, 5000.0);

  //glClearColor(0.5, 0.5, 0.5, 1.0);

  glLightfv (GL_LIGHT0, GL_POSITION, position);
  glLightfv (GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv (GL_LIGHT0, GL_SPECULAR, diffuse);
  glLightfv (GL_LIGHT0, GL_AMBIENT, ambient);

  glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, spotdif);
  glLightfv(GL_LIGHT1, GL_SPECULAR, spotdif);
  glLightfv(GL_LIGHT1, GL_AMBIENT, spotamb);
  //glLightfv(GL_LIGHT1, GL_POSITION, spotpos);
  //glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotdir);

   makeCheckImage();
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glGenTextures(1, &texName);
   glBindTexture(GL_TEXTURE_2D, texName);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight,
                0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glBindTexture(GL_TEXTURE_2D, texName);

  //glMap2f(GL_MAP2_VERTEX_3,
  //	  0, 1, 3, 5,
  //	  0, 1, 15, 5,
  //	  &sandpoints[0][0][0]);
  glEnable(GL_MAP2_VERTEX_3);
  //glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);

  for(i=0;i<5;i++){
    for(j=0;j<5;j++){
      surfaceangles[i][j] = rand()%360;
    }
  }

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
 /*
  //optimize the collision detection
  int a = 0, count = 0;
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
  	  if(fabs(fish[i].location[1]-fish[a].location[1])<70 && i != a) {
  	  	fish[i].planeFish[count] = fish+a;
  	  	count++;
  	  }
  	}
  	//printf("count = %i\n", count);
  }
  */
   GLfloat sandpoints[5][5][3] = {
    { { (xmin-20),   80.0, (zmin-20) },
      { (xmin-20)/2, 0.0, (zmin-20) },
      { (xmax+20),   60.0, (zmin-20) },
      { (xmax+20)/2, 20.0, (zmin-20) },
      { (xmax+20),   0.0, (zmin-20) }},

    { { (xmin-20),   0.0, (zmin-20)/2 },
      { (xmin-20)/2, 60.0, (zmin-20)/2 },
      { (xmax+20),   0.0, (zmin-20)/2 },
      { (xmax+20)/2, 80.0, (zmin-20)/2 },
      { (xmax+20),   20.0, (zmin-20)/2 }},

    { { (xmin-20),   40.0, 0    },
      { (xmin-20)/2, 0.0, 0    },
      { (xmax+20),   20.0, 0 },
      { (xmax+20)/2, 40.0, 0    },
      { (xmax+20),   80.0, 0    }},

    { { (xmin-20),   20.0,  (zmax+20)/2 },
      { (xmin-20)/2, 40.0,  (zmax+20)/2 },
      { (xmax+20),   80.0,  (zmax+20)/2 },
      { (xmax+20)/2, 0.0,  (zmax+20)/2 },
      { (xmax+20),   20.0,  (zmax+20)/2 }},

    { { (xmin-20),   0.0,  (zmax+20) },
      { (xmin-20)/2, 0.0,  (zmax+20) },
      { (xmax+20),   0.0,  (zmax+20) },
      { (xmax+20)/2, 0.0,  (zmax+20) },
      { (xmax+20),   0.0,  (zmax+20) }}
  };

  sandList = glGenLists(1);
  glNewList(sandList, GL_COMPILE);
     glMap2f(GL_MAP2_VERTEX_3,
	   0, 1, 3, 5,
	   0, 1, 15, 5,
	   &sandpoints[0][0][0]);
   glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
   glEvalMesh2(GL_FILL, 0, 20, 0, 20);
   glEndList();

  init_fish();
  init_castle();
  init_lighthouse();

  cm = new collisionMap(FISHCOUNT, &fish);

  //Shader part, don't know if i'm going to use it yet
/*  bool shaderSupported = IsExtensionSupported("GL_ARB_vertex_shader");
	if(shaderSupported) {
		printf("shader is supported\n");
		GLubyte* name = (GLubyte*)"glCreateShaderObjectARB";
		typedef GLuint (*enumFunction)(GLenum);
		enumFunction function = NULL;
		function = (enumFunction)glXGetProcAddressARB(name);
		if(function)
			printf("found create object\n");
		GLuint vertexShader = (*function)(GL_VERTEX_SHADER);
		printf("vertexShader = %i\n", vertexShader);

	}*/

}

void read_file(){
  //char filename[] = "/tmp/fishinput";
  //FILE *file;
  //double number;
  //int count;
  static bool NewThread = false;

  int i = 0;
  for(i = 0; i < FISHCOUNT; i++)
  	fish[i].checked = false;

  com.Start(NewThread);
  NewThread = true;
 // count = 0;

  //check if fish were read in the xml


 // file = fopen(filename, "r");

 /* while(fscanf(file, "%lf\n", &number) != EOF){
  //  printf("Read %f of %i\n", number, count);
    if(number < -2.0){
      fish[count].alive = 0;
      fish[count].finrotation = 40;
      fish[count].oldfinrotation[0] = -10.5;
      fish[count].oldfinrotation[1] = -10.75;
      fish[count].oldfinrotation[2] = -11.0;
      fish[count].blinking = EYE_CLOSED;
      fish[count].targetsize = 1;
    }else{
      if(fish[count].alive == 0){
	fish[count].finrotation = 0.0;
	fish[count].oldfinrotation[0] = 0.0;
	fish[count].oldfinrotation[1] = 0.0;
	fish[count].oldfinrotation[2] = 0.0;
      }
      fish[count].alive = 1;
      //changed to go to xml
      //fish[count].targetsize = number;
      //fish[count].targetsize = (fish[count].load_one / (double)fish[count].cpus);
//      printf("target size %i = %f\n", count, fish[count].targetsize);
    }
    count++;
  }
  fclose(file);
*/
}

/*
void read_file(){
  char filename[] = "/tmp/fishinput";
  FILE *file;
  double number;
  int count;

  count = 0;
  file = fopen(filename, "r");
  while((fscanf(file, "%lf\n", &number) != EOF) && (count < FISHCOUNT)){
    //printf("Read %f\n", number);
    fish[count].targetsize = number;
    count++;
  }
  fclose(file);
}
*/


void draw_bubbles(){



  int i;
  GLfloat bubbleamb[] = { 1, 1, 1, .7 };
  GLfloat bubbledif[] = { 1, 1, 1, .6 };
  GLfloat bubblespec[] = { 1, 1, 1, .8 };
  GLfloat shiny = 100.0f;

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, bubbleamb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, bubbledif);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, bubblespec);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shiny);


  static int madeBubbleList = 0;
  if(!madeBubbleList) {
  	bubbleList = glGenLists(1);
  	glNewList(bubbleList, GL_COMPILE);
#ifdef WIN32
	GLUquadricObj* obj;
	obj = gluNewQuadric();
	gluSphere(obj, 5, 10, 10);
#else
  	glutSolidSphere(5, 10, 10);
#endif
  	glEndList();
  	madeBubbleList = 1;
  }
startBubbleShaders();
  for(i=0;i<BUBBLECOUNT;i++){
    glPushMatrix();
    //glTranslatef(bubbles[i].location[0] + sin(bubbles[i].location[1]), bubbles[i].location[1], bubbles[i].location[2]);
    glTranslated(bubbles[i].location[0], bubbles[i].location[1], bubbles[i].location[2]);

	glCallList(bubbleList);
    glPopMatrix();
    //notice getTime returns the elapsed time since last update time, or iteration
    //bubbles[i].location[0] += sin(bubbles[i].location[1]);
    bubbles[i].location[1] += bubbles[i].speed* getTime()*BUBBLESPEED;
    if(bubbles[i].location[1] > ymax){
      bubbles[i].location[1] = -ymax;
    }
  }
  stopBubbleShaders();
}


void draw_fish_simple(int fishnum)
{
  GLfloat bodyamb[] = { 1, 0.6, 0.0, 1 };
  GLfloat bodydif[] = { 1, 0.6, 0.0, 1 };
  GLfloat bodyspec[] = { 0.0, 0.0, 0.0, 1 };

  GLfloat eyeamb[] = { 1, 1, 1, 1.0 };
  GLfloat eyedif[] = { 1, 1, 1, 1.0 };
  GLfloat eyespec[] = { 1, 1, 1, 1.0 };

  GLfloat shine = 0;

  glPushMatrix();
  glScalef(.3, 1, 1.5);
  glScalef(fish[fishnum].size+1.0, fish[fishnum].size+1.0, fish[fishnum].size+1.0);

  glMaterialfv(GL_FRONT, GL_AMBIENT, bodyamb);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, bodydif);
  glMaterialfv(GL_FRONT, GL_SPECULAR, bodyspec);
  glMaterialf(GL_FRONT, GL_SHININESS, shine);

#ifdef WIN32
  GLUquadricObj* obj;
  obj = gluNewQuadric();
  gluSphere(obj, 10, 15, 15);
#else
  glutSolidSphere(10, 15, 15);
#endif

  glPushMatrix();
    glRotated(fish[fishnum].finrotation, 0, 1, 0);
    glTranslated(0.0, 0.0, -13);
#ifdef WIN32
	//yet to impliment, function for cone

#else
    glutSolidCone(10, 5, 15, 15);
#endif
  glPopMatrix();

  if(fish[fishnum].alive)
    fish[fishnum].finrotation += fish[fishnum].findirection*fish[fishnum].speed;

  if(fish[fishnum].finrotation > 8){
    fish[fishnum].findirection = -1;
  }
  else if(fish[fishnum].finrotation < -8){
    fish[fishnum].findirection = 1;
  }


  glMaterialfv(GL_FRONT, GL_AMBIENT, eyeamb);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, eyedif);
  glMaterialfv(GL_FRONT, GL_SPECULAR, eyespec);
  glMaterialf(GL_FRONT, GL_SHININESS, shine);

  glPopMatrix();
}


void update_surface(GLfloat ***surfacepoints)
{
  int i, j, angle=0;

  for(i=0;i<5;i++){
    for(j=0;j<5;j++){
      surfacepoints[i][j][1] += sin(angle * degrad);
    }
  }

  angle = (angle+1)%360;
}

void moveCamera(int code) {
	static double eyeX = 0, eyeY = 0, eyeZ = 2000.0, centerX = 0.0, centerY = 0.0, centerZ = 0.0, upX = 0.0, upY = 1.0, upZ = 0.0;
	static double eyeZcenter = 2000.0;
	int i = 0;
	if(code==1) {
		eyeZcenter+=20.0;
		return;
	}
	else if(code==2) {
		eyeZcenter-=20.0;
		return;
	} else if(code==3) {
		eyeZcenter=2000.0;
		return;
	}
	for(i = 0; i < FISHCOUNT; i++) {

		if(fish[i].picked)
			break;


	}

	if(i == FISHCOUNT) {

		if(cameraSelected == CASTLESELECTED) {
		renderWalls = false;
		static float angleCounter = 0;
		angleCounter += getTime() * 40;
		eyeX += ((bubblerloc[0] -cos((PI*angleCounter)/180)*1000)-eyeX)/15;
		eyeY += ((ymin+200)-eyeY)/10;
		eyeZ += ((bubblerloc[2]-sin((PI*angleCounter)/180)*1000) - eyeZ)/15;
		centerX += (bubblerloc[0]+75-centerX)/10;
		centerY += (ymin+200-centerY)/10;
		centerZ += (bubblerloc[2]-centerZ)/10;
		upX = 0;
		upY = 100000;
		upZ = -5000;

		gluLookAt (eyeX, eyeY, eyeZ, // Eye
	     centerX, centerY, centerZ,   // Center
	     upX, upY, upZ);  // Up



		} else {
		renderWalls = true;
					eyeX += (0-eyeX)/10;
		eyeY += (0-eyeY)/10;
		eyeZ += (eyeZcenter-eyeZ)/10;
		centerX += (0-centerX)/10;
		centerY += (0-centerY)/10;
		centerZ += (0-centerZ)/10;
		upX += (0-upX)/10;
		upY += (1-upY)/10;
		upZ += (0-upZ)/10;
		gluLookAt (eyeX, eyeY, eyeZ, // Eye
	     centerX, centerY, centerZ,   // Center
	     upX, upY, upZ);  // Up
				}
	} else {
		if(cameraCounter > CAMERATIMEOUT) {
			cameraCounter = 0;
			fish[i].picked = false;
			fishstats.selectFish(-1);
		}

		eyeX += (fish[i].location[0]-eyeX)/10;
		eyeY += (fish[i].location[1]-eyeY)/10;
		eyeZ += ((fish[i].location[2]+300)-eyeZ)/10;
		centerX += (fish[i].location[0]-centerX)/10;
		centerY += (fish[i].location[1]-centerY)/10;
		centerZ += (fish[i].location[2]-centerZ)/10;
		upX = 0;
		upY = 100000;
		upZ = -5000;

		gluLookAt (eyeX, eyeY, eyeZ, // Eye
	     centerX, centerY, centerZ,   // Center
	     upX, upY, upZ);  // Up


	}


}


/*float collision_avoidance(int fishnum)
{
  int i, j;
  double dist, angletoi;
  double angle;
  int thresh = 30.0;
	fish[fishnum].colliding = 0;
  for(i=0;i<FISHCOUNT;i++){
    if(i != fishnum){

	dist = sqrt(pow(fish[i].location[0] - fish[fishnum].location[0], 2.0) +
		    pow(fish[i].location[2] - fish[fishnum].location[2], 2.0));

	if(abs(fish[i].location[1] - fish[fishnum].location[1]) < 70) {
	angle = atan((fish[i].location[2] - fish[fishnum].location[2])/ (fish[i].location[0] - fish[fishnum].location[0]));
	angle = angle - fish[fishnum].rotation;
	fish[fishnum].rotation = fish[fishnum].rotation + (2/(dist-((fish[i].size + fish[fishnum].size)*34)))*tan(angle)*(180/3.242);
	}
	//hitting eachother's personal sphere
	if(dist < (34*fish[i].size + 34*fish[fishnum].size)) {



//		printf("found collision\n");
//		fflush(stdout);
		fish[fishnum].colliding = 1;

//	if(dist < ((fish[i].size+1) * thresh)+((fish[fishnum].size+1) * thresh)){
	  //printf("Sizes: %f  %f  Dist: %f\n", fish[fishnum].size, fish[i].size, dist);
	  //printf("Checking %d against %d: %f\n", fishnum, i, dist);

/*	  angletoi = fmod(((atan2(fish[fishnum].location[0]-fish[i].location[0],
				  fish[fishnum].location[2]-fish[i].location[2])
			    /degrad)+360.0), 360.0);

	/*
//	  if(fmod(((angletoi - (fmod((fish[fishnum].rotation+360.0) + 135.0, 360.0))))+360.0, 360.0) < 90){
	    //printf("--------\n");
	    //printf("My angle:           %f\n", fish[fishnum].rotation);
	    //printf("My corrected angle: %f\n",(fmod((fish[fishnum].rotation+360.0) + 135.0, 360.0)));
	    //printf("Angle to %d:         %f\n", i, angletoi);
	    /*
	    glBegin(GL_LINES);
	    glVertex3f(fish[fishnum].location[0], fish[fishnum].location[1], fish[fishnum].location[2]);
	    glVertex3f(fish[i].location[0], fish[i].location[1], fish[i].location[2]);
	    glEnd();
	    fish[fishnum].colliding = 1;
	    */
	    //return 0.0;
//	    return pow(dist/(((fish[i].size+1) * thresh)+((fish[fishnum].size+1) * thresh)),2);
//	  }
	  //printf("Too close!\n");
//	}
/*      }
    }
  }
  //fish[fishnum].colliding = 0;
  //return 1.0;
}
*/


void display(void)
{
	glLoadIdentity();

	if(!picking)
		updateTime();
  int i, j;
  static double statcount = 0;
  static int angle = 0;
  static int dark;
  /*
  float fog_tank = .0002;
  float fog_reflect = .0015;
  */
  float fog_tank = .0003;
  float fog_reflect = .0008;

	//no longer needed because of sandList
/*  static GLfloat sandpoints[5][5][3] = {
    { { (xmin-20),   80.0, (zmin-20) },
      { (xmin-20)/2, 0.0, (zmin-20) },
      { (xmax+20),   60.0, (zmin-20) },
      { (xmax+20)/2, 20.0, (zmin-20) },
      { (xmax+20),   0.0, (zmin-20) }},

    { { (xmin-20),   0.0, (zmin-20)/2 },
      { (xmin-20)/2, 60.0, (zmin-20)/2 },
      { (xmax+20),   0.0, (zmin-20)/2 },
      { (xmax+20)/2, 80.0, (zmin-20)/2 },
      { (xmax+20),   20.0, (zmin-20)/2 }},

    { { (xmin-20),   40.0, 0    },
      { (xmin-20)/2, 0.0, 0    },
      { (xmax+20),   20.0, 0 },
      { (xmax+20)/2, 40.0, 0    },
      { (xmax+20),   80.0, 0    }},

    { { (xmin-20),   20.0,  (zmax+20)/2 },
      { (xmin-20)/2, 40.0,  (zmax+20)/2 },
      { (xmax+20),   80.0,  (zmax+20)/2 },
      { (xmax+20)/2, 0.0,  (zmax+20)/2 },
      { (xmax+20),   20.0,  (zmax+20)/2 }},

    { { (xmin-20),   0.0,  (zmax+20) },
      { (xmin-20)/2, 0.0,  (zmax+20) },
      { (xmax+20),   0.0,  (zmax+20) },
      { (xmax+20)/2, 0.0,  (zmax+20) },
      { (xmax+20),   0.0,  (zmax+20) }}
  };
	*/

  static GLfloat surfacepoints[5][5][3] = {
    { { (xmin-20),   5.0, (zmin-20) },
      { (xmin-20)/2, 0.0, (zmin-20) },
      { (xmax+20),   5.0, (zmin-20) },
      { (xmax+20)/2, 0.0, (zmin-20) },
      { (xmax+20),   2.0, (zmin-20) }},

    { { (xmin-20),   0.0, (zmin-20)/2 },
      { (xmin-20)/2, 5.0, (zmin-20)/2 },
      { (xmax+20),   2.0, (zmin-20)/2 },
      { (xmax+20)/2, 0.0, (zmin-20)/2 },
      { (xmax+20),   0.0, (zmin-20)/2 }},

    { { (xmin-20),   0.0, 0 },
      { (xmin-20)/2, 0.0, 0 },
      { (xmax+20),   5.0, 0 },
      { (xmax+20)/2, 0.0, 0 },
      { (xmax+20),   2.0, 0 }},

    { { (xmin-20),   0.0,  (zmax+20)/2 },
      { (xmin-20)/2, 5.0,  (zmax+20)/2 },
      { (xmax+20),   0.0,  (zmax+20)/2 },
      { (xmax+20)/2, 2.0,  (zmax+20)/2 },
      { (xmax+20),   0.0,  (zmax+20)/2 }},

    { { (xmin-20),   0.0,  (zmax+20) },
      { (xmin-20)/2, 5.0,  (zmax+20) },
      { (xmax+20),   0.0,  (zmax+20) },
      { (xmax+20)/2, 2.0,  (zmax+20) },
      { (xmax+20),   0.0,  (zmax+20) }}
  };


  GLfloat glassamb[] =  { 0.3, 0.3, 0.3, 1 };
  GLfloat glassdif[] =  { 0.3, 0.3, 0.3, .3 };
  GLfloat glassspec[] = { 0.1, 0.1, 0.1, 1 };

  GLfloat sandamb[] =  { 0.92, 0.68, 0.43, 1 };
  GLfloat sanddif[] =  { 0.92, 0.68, 0.43, 1 };
  GLfloat sandspec[] = { 1, 1, 1, 1 };

  GLfloat surfaceamb[] =  { 0.1, 0.1, 0.9, .1 };
  GLfloat surfacedif[] =  { 0.1, 0.1, 0.9, .1 };
  GLfloat surfacespec[] = { 0.1, 0.1, 0.9, .1 };

  int c;
  GLfloat shine = 5;
  static double accessCount = 0;
  if(!picking) {
  //if(accessCount > 10) {

 // if(access("/tmp/fishdark", F_OK) == 0){

 if(!nightTime)
 	dark = 1;
 else
 	dark = 0;

    if(!dark){
      glDisable(GL_LIGHT0);
      glEnable(GL_LIGHT1);
      dark = 1;

 // }else{
    if(dark){

      dark = 0;
    }
 // }
  accessCount = 0;
  //problems
  for(c = 0; c < FISHCOUNT; c++)
  	fish[c].blinking = EYE_CLOSED;

  	night = 1;
  } else {
  	     glEnable(GL_LIGHT0);
      glDisable(GL_LIGHT1);
  	if(night) {
  		for(c = 0; c < FISHCOUNT; c++)
  			fish[c].blinking = EYE_OPEN;
  	}
  	night = 0;
  }
 /* } else {
  	accessCount += getTime();


  }*/

  frames++;

  if(statcount > FILEREFRESH){
    read_file();
    /*for(i = 0; i < FISHCOUNT; i++)
  	  fish[i].targetsize = ((float)(rand()%220))/100.0;
    fish[0].targetsize = 9.7;
   */ statcount = 0;
  }else{
    statcount += getTime();
  }
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glClear(GL_DEPTH_BUFFER_BIT);
  glPushMatrix ();
	//weird that you have to call this every frame, hum?
	//need to look into it more

//	gluLookAt(0.0, 2000.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

	moveCamera(0);


 /* gluLookAt (0.0, 0.0, 2000.0, // Eye
	     0.0, 0.0, 0.0,   // Center
	     0.0, 1.0, 0.0);  // Up

	*/




	image_incriment();


   glPushMatrix ();

	//fish
   for(i=0;i<FISHCOUNT;i++){
  //   collision_avoidance(i);



 /*  for(j=MODEL_REFLECT_LEFT;j<=MODEL_REAL;j++){

       if(j == MODEL_REFLECT_LEFT){
	 if(fish[i].location[0] > (xmin-xmin/2))
	   continue;
	 glPushMatrix();
	 glTranslatef(xmin-20, 0, 0);
	 glScalef(-1.0, 1.0, 1.0);
	 glTranslatef(xmax+20, 0, 0);
	 glFogf(GL_FOG_DENSITY, fog_reflect);
       }
       else if(j == MODEL_REFLECT_RIGHT){
	 if(fish[i].location[0] < (xmax-xmax/2))
	   continue;
	 glPushMatrix();
	 glTranslatef(xmax+20, 0, 0);
	 glScalef(-1.0, 1.0, 1.0);
	 glTranslatef(xmin-20, 0, 0);
	 glFogf(GL_FOG_DENSITY, fog_reflect);
       }
       else if(j == MODEL_REFLECT_BACK){
	 glPushMatrix();
	 glTranslatef(0, 0, zmin-20);
	 glScalef(1.0, 1.0, -1.0);
	 glTranslatef(0, 0, zmax+20);
	 glFogf(GL_FOG_DENSITY, fog_reflect);

       }
  *///     else if(j == MODEL_REAL){
	 glPushMatrix();
	 glFogf(GL_FOG_DENSITY, fog_tank);

 //      }
       //conllision geometry here

       //glTranslatef(fish[i].location[0], fish[i].location[1]*((30.0 - fish[i].size)/30.0), fish[i].location[2]);
       glTranslatef(fish[i].location[0], fish[i].location[1], fish[i].location[2]);
       #ifdef COLLISIONDEBUG
		glPushMatrix();

		 	              glutWireSphere((fish[i].size+1)*34+100, 10, 10);
		glPopMatrix();
		#endif //COLLISIONDEBUG
       glRotated(fish[i].rotation, 0.0, 1.0, 0.0);

       if(!fish[i].alive){
	 if(fish[i].deathangle < 90){
	   fish[i].dying = 1;
	 }
	 if(fish[i].dying){
	   fish[i].deathangle += 5;
	   if(fish[i].deathangle >= 90){
	     fish[i].deathangle = 90;
	     fish[i].dying = 0;
	   }
	 }
	 glRotated(fish[i].deathangle, 0, 0, 1);
       }

       if(fish[i].alive){
	 if(fish[i].deathangle > 0){
	   fish[i].undying = 1;
	 }
	 if(fish[i].undying){
	   fish[i].deathangle -= 5;
	   if(fish[i].deathangle <= 0){
	     fish[i].deathangle = 0;
	     fish[i].undying = 0;
	   }
	   glRotated(fish[i].deathangle, 0, 0, 1);
	 }
	   }

	   //turn on and off the shaders
		   startFishShaders();
       draw_fish(i);
		   stopFishShaders();

       glPopMatrix();
   //}



   double nightfactor = 1;
	if(night)
		nightfactor = NIGHTFACTOR;



     if(fish[i].alive){
       /* Fishies turn in circles */
       #ifdef COLLISION_H
       if(!picking && (fish[i].alive==1))
       collision_avoidance(i);


       #endif //COLLISION_H
    //   if(abs(fish[i].rotation-fish[i].targetrotation) > 5)
    //Rotation Add part
    //really buggy
   // if(abs(fish[i].targetrotation - fish[i].rotation) < 1)
  //  	fish[i].rotation += .5;

 /*   printf("----------------------------\n");
    printf("rotation = %lf\n", fish[i].rotation);
    printf("targetrotation = %lf\n", fish[i].targetrotation);
    fflush(stdout);
   */

    //make sure that all of the angle are between 0 and 360
    if(!picking) {
    	// if(fish[i].picked)
    	//fish[i].targetrotation = 90;
           if(fish[i].rotation < 0)
       	fish[i].rotation += 360;

       fish[i].rotation = fmod(fish[i].rotation, 360);

       if(fish[i].targetrotation < 0)
       	fish[i].targetrotation += 360;

       fish[i].targetrotation = fmod(fish[i].targetrotation, 360);
    //still really buggy, figure out
    if(night)
    	fish[i].rotationspeed = .1;
    else
    	fish[i].rotationspeed = 1;



   	if(fabs(fish[i].targetrotation - fish[i].rotation) > 180)
   		fish[i].rotation -= (fish[i].targetrotation - fish[i].rotation) * fish[i].rotationspeed * ROTATIONSPEED * getTime();// + ((rand()%300)/100);
   	else
       	fish[i].rotation += (fish[i].targetrotation - fish[i].rotation) * fish[i].rotationspeed * ROTATIONSPEED * getTime() + ((rand()%300)/100);





       /*
        *
       if(rand()%2){
	 fish[i].rotation = fmod(((fish[i].rotation + (nightfactor * fish[i].direction * getTime() * ROTATIONSPEED *(rand() % 2))/(fish[i].size+1)))+360.0, 360.0);
       }else{
	 //printf("%d size = %f\n", i, fish[i].size);
*/
       //fish[i].rotation = 0;

   	double oldLoc[3];
   	oldLoc[0] = fish[i].location[0];
   	oldLoc[1] = fish[i].location[1];
   	oldLoc[2] = fish[i].location[2];

    	   if(fabs(fish[i].targetrotation - fish[i].rotation) > 90)
    	   		fish[i].speed = 1.5;
    	   	else
    	   		fish[i].speed = 2.5;
       /* Fishies swim around */
       fish[i].location[0] = fish[i].location[0] + ((nightfactor * fish[i].speed * getTime() * FISHSPEED * sin(fish[i].rotation*(degrad)))/(fish[i].size+1.0));
       fish[i].location[2] = fish[i].location[2] + ((nightfactor * fish[i].speed * getTime() * FISHSPEED * cos(fish[i].rotation*(degrad)))/(fish[i].size+1.0));


	//speed when moving up or down
       if(fish[i].location[1] > fish[i].homedepth && !fish[i].picked){
	 fish[i].location[1] += (fish[i].homedepth-fish[i].location[1])/10;
       } else if(fish[i].location[1] < fish[i].homedepth && !fish[i].picked){
       	 fish[i].location[1] += (fish[i].homedepth-fish[i].location[1])/10;
       } else if (fish[i].picked) {
   //    		fish[i].location[1] += (-300-fish[i].location[1])/10;

       }

       if(fish[i].location[0] > xmax-(fish[i].size+1)*30){
	 fish[i].location[0] = xmax-(fish[i].size+1)*30;
       }
       if(fish[i].location[0] < xmin+(fish[i].size+1)*30){
	 fish[i].location[0] = xmin+(fish[i].size+1)*30;
       }

       if(fish[i].location[2] > zmax-(fish[i].size+1)*30){
	 fish[i].location[2] = zmax-(fish[i].size+1)*30;
       }
       if(fish[i].location[2] < zmin+(fish[i].size+1)*30){
	 fish[i].location[2] = zmin+(fish[i].size+1)*30;
       }

       /* Test if the fish are stationary */
       double dist = pow(fish[i].location[0] - oldLoc[0],2) + pow(fish[i].location[2] - oldLoc[2], 2);
       if(dist < _MINSPEED) {
           fish[i].location[0] = 0.0;
           fish[i].location[2] = 0.0;

       }


       /* Do some fin wiggling */
       fish[i].oldfinrotation[2] = fish[i].oldfinrotation[1];
       fish[i].oldfinrotation[1] = fish[i].oldfinrotation[0];
       fish[i].oldfinrotation[0] = (15-fabs(fish[i].finrotation))*fish[i].findirection;

		//so that the first iteration, where getTime is invalid doesn't mess it up
	   //may not be valid anymore, since the main function now validates the output
		if(fabs(getTime()) > 1)
			fish[i].finrotation += fish[i].findirection;
		else
       		fish[i].finrotation += nightfactor * fish[i].findirection * getTime() * FINSPEED;
       if(fish[i].finrotation > 15){
	 //fish[i].finrotation = 15;
	 fish[i].findirection = -1;
       }
       else if(fish[i].finrotation < -15){
	 //fish[i].finrotation = -15;
	 fish[i].findirection = 1;
       }
	 if(fabs(fish[i].finrotation) > 20) {

	 	fish[i].finrotation = fish[i].findirection * 20;
	 	fish[i].findirection *= -1;

       }


       /* Create some darting */
       if(fish[i].speed > 1){
	 fish[i].speed -= .05 * getTime() * DARTDURATION;
	 //fish[i].location[1] = fish[i].location[1] + (0.5)*((float)fish[i].ydirection);
       }else{
	 if((rand() % 500) == 10){
	   fish[i].speed = 4;
	   fish[i].ydirection = (rand()%3)-1;
	 }
       }
     }
 }
     /* Dead fish float to the top */
     if(!fish[i].alive){
       if(fish[i].location[1] < ymax-5){
	 fish[i].location[1] += 1;
       }
       else{
	 fish[i].finrotation = 40 - 10*cos((fish[i].bobcounter+30) * degrad);
	 fish[i].location[1] += .10 * sin(fish[i].bobcounter * degrad);
	 fish[i].bobcounter = (fish[i].bobcounter + 1) % 360;
       }
     }

     if(!picking) {
     if(!night) {
     /* We all like blinking fish */
     if(fish[i].alive){
       if(((rand() % 1000) < 5) && fish[i].blinking == EYE_OPEN){
	 fish[i].blinking = EYE_HALF_CLOSED;
	 fish[i].blinkcount = 0;
       }

       //Determine next eye state, notice, time dependent
       if(fish[i].blinking != EYE_OPEN){
		 if(fish[i].blinkcount < 10)
	   		fish[i].blinking = EYE_CLOSED;
	   	else if(fish[i].blinkcount < 20)
	   		fish[i].blinking = EYE_HALF_OPEN;
	   	else if(fish[i].blinkcount < 30)
	   		fish[i].blinking = EYE_OPEN;
	   	else if(fish[i].blinkcount > 30)
	   		fish[i].blinking = EYE_OPEN;

	   	//always do
       	fish[i].blinkcount += getTime() * BLINKSPEED;
       }
       }
     }




     /* Fish sometimes change size */
     //if(fish[i].alive){

       if((fish[i].targetsize - fish[i].size) > 0.05){
	 fish[i].size += GROWTHFACTOR * getTime();
       }
       else if((fish[i].targetsize - fish[i].size) < -0.05){
	 fish[i].size -= GROWTHFACTOR * getTime();
       }
       //}



     /* FIXME: Remove this after ganglia is connected */

    /*   if((rand() % 10000) == 1){
       if(fish[i].alive){
       fish[i].alive = 0;
       }else{
       fish[i].alive = 1;
       }
       }
     */
   }
   }


   glPopMatrix ();

	if(!picking)
   draw_bubbles();

	static int castleList = 0;

   /* Castle */
   if(!picking) {

   	if(!castleList) {
   		castleList = glGenLists(1);
   		//draw_castle();
   		glNewList(castleList, GL_COMPILE);
		   glPushMatrix();
 /*  for(i=MODEL_REFLECT_LEFT;i<=MODEL_REAL;i++){
     glPushMatrix();
     if(i == MODEL_REFLECT_LEFT){
       glTranslatef(xmin-20, 0, 0);
       glScalef(-1.0, 1.0, 1.0);
       glTranslatef(xmax+20, 0, 0);
       glFogf(GL_FOG_DENSITY, fog_reflect);
     }
     else if(i == MODEL_REFLECT_RIGHT){
       glTranslatef(xmax+20, 0, 0);
       glScalef(-1.0, 1.0, 1.0);
       glTranslatef(xmin-20, 0, 0);
       glFogf(GL_FOG_DENSITY, fog_reflect);
     }
     else if(i == MODEL_REFLECT_BACK){
       glTranslatef(0, 0, zmin-20);
       glScalef(1.0, 1.0, -1.0);
       glTranslatef(0, 0, zmax+20);
       glFogf(GL_FOG_DENSITY, fog_reflect);
     }
     else if(i == MODEL_REAL){
*/       glFogf(GL_FOG_DENSITY, fog_tank);
//     }
     //glTranslatef(bubblerloc[0], ymin+190, bubblerloc[2]);
     glTranslatef(bubblerloc[0], ymin+130, bubblerloc[2]-100);

     draw_castle();


	     glTranslatef(0, 0, 100);
  //   glutSolidSphere(300, 30, 30);

     //glutSolidSphere(80, 20, 20);
     glPopMatrix();
  // }
   	glEndList();
   	} else {

   		glCallList(castleList);

   	}
   } else {
   	glPushMatrix();
   	 glTranslatef(bubblerloc[0], ymin+130, bubblerloc[2]-100);
	glPushName(FISHCOUNT+1);
     draw_castle();
     glPopName();

	     //glTranslatef(0, 0, 100);
	     glPopMatrix();
   }


   /* Lighthouse */
   if(!picking) {




 //  for(i=MODEL_REFLECT_LEFT;i<=MODEL_REAL;i++){
     glPushMatrix();
   /*  if(i == MODEL_REFLECT_LEFT){
       glTranslatef(xmin-20, 0, 0);
       glScalef(-1.0, 1.0, 1.0);
       glTranslatef(xmax+20, 0, 0);
       glFogf(GL_FOG_DENSITY, fog_reflect);
     }
     else if(i == MODEL_REFLECT_RIGHT){
       glTranslatef(xmax+20, 0, 0);
       glScalef(-1.0, 1.0, 1.0);
       glTranslatef(xmin-20, 0, 0);
       glFogf(GL_FOG_DENSITY, fog_reflect);
     }
     else if(i == MODEL_REFLECT_BACK){
       glTranslatef(0, 0, zmin-20);
       glScalef(1.0, 1.0, -1.0);
       glTranslatef(0, 0, zmax+20);
       glFogf(GL_FOG_DENSITY, fog_reflect);
     }
 *///    else if(i == MODEL_REAL){
       glFogf(GL_FOG_DENSITY, fog_tank);
  //   }
     //glTranslatef(bubblerloc[0], ymin+190, bubblerloc[2]);
     glTranslatef(-bubblerloc[0], ymin+(325), 0);
     glScalef(5, 5, 5);
     draw_lighthouse();


     //draw_castle();
     //glutSolidSphere(80, 20, 20);
     glPopMatrix();
  // }



   } else {
   	glPushMatrix();
   	     glTranslatef(-bubblerloc[0], ymin+(325), 0);
     glScalef(5, 5, 5);
     glPushName(FISHCOUNT+2);
     draw_lighthouse();
     glPopName();
   	glPopMatrix();

   }

	if(!picking) {
   /* Sand */
   //outdated since made sand into display list
/*   glMap2f(GL_MAP2_VERTEX_3,
	   0, 1, 3, 5,
	   0, 1, 15, 5,
	   &sandpoints[0][0][0]);
   glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
 */
 	static int sandList2 = 0;
 	if(!sandList2) {
 		sandList2 = glGenLists(1);
 		glNewList(sandList2, GL_COMPILE);
   glMaterialfv(GL_FRONT, GL_AMBIENT, sandamb);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, sanddif);
   glMaterialfv(GL_FRONT, GL_SPECULAR, sandspec);

   for(i=MODEL_REFLECT_LEFT;i<=MODEL_REAL;i++){
     glPushMatrix();
     if(i == MODEL_REFLECT_LEFT){
       glTranslatef(xmin-20, 0, 0);
       glScalef(-1.0, 1.0, 1.0);
       glTranslatef(xmax+20, 0, 0);
       glFogf(GL_FOG_DENSITY, fog_reflect);
     }
     else if(i == MODEL_REFLECT_RIGHT){
       glTranslatef(xmax+20, 0, 0);
       glScalef(-1.0, 1.0, 1.0);
       glTranslatef(xmin-20, 0, 0);
       glFogf(GL_FOG_DENSITY, fog_reflect);
     }
     else if(i == MODEL_REFLECT_BACK){
       glTranslatef(0, 0, zmin-20);
       glScalef(1.0, 1.0, -1.0);
       glTranslatef(0, 0, zmax+20);
       glFogf(GL_FOG_DENSITY, fog_reflect);
     }
     else if(i == MODEL_REAL){
       glFogf(GL_FOG_DENSITY, fog_tank);
     }
     glTranslatef(0, ymin-10, 0);
//   glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	 glCallList(sandList);
     glPopMatrix();
   }
   glEndList();
 	} else {
 		glCallList(sandList2);
 	}


  /* Walls */
  /* ----- */
  glMaterialfv(GL_FRONT, GL_AMBIENT, glassamb);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, glassdif);
  glMaterialfv(GL_FRONT, GL_SPECULAR, glassspec);
  glMaterialf(GL_FRONT, GL_SHININESS, shine);

  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glFogf(GL_FOG_DENSITY, fog_tank);
  glShadeModel(GL_FLAT);

  /* Back */
  glPushMatrix();

	startWallShaders();

  static int madeWallList = 0;

  if(!madeWallList) {
  	wallList = glGenLists(1);
  	glNewList(wallList, GL_COMPILE);
  	//attempt at making a array for drawarrays
 /* 	static float wallArray[72] = {	xmin-20, ymax+100, zmin-20,
  									xmin-20, ymin-20, zmin-20,
  									xmax+20, ymin-20, zmin-20,
  									xmax+20, ymax+100, zmin-20,

  									xmin-40, ymax+100, zmin-40,
  									xmin-40, ymin-40, zmin-40,
  									xmax+40, ymin-40, zmin-40,
  									xmin-20, ymax+100, zmin-20,

  									xmin-20, ymax+100, zmax+20,
  									xmin-20, ymin-20, zmax+20,
  									xmin-20, ymin-20, zmin-20,
  									xmin-20, ymax+100, zmin-20,

  									xmin-40, ymax+100, zmax+40,
  									xmin-40, ymin-40, zmax+40,
  									xmin-40, ymin-40, zmin-40,
  									xmin-40, ymax+100, zmin-40,

  									xmax+20, ymax+100, zmin-20,
  									xmax+20, ymin-20, zmin-20,
  									xmax+20, ymin-20, zmax+20,
  									xmax+20, ymax+100, zmax+20,

  									xmax+40, ymax+100, zmin-40,
  									xmax+40, ymin-40, zmin-40,
  									xmax+40, ymin-40, zmax+40,
  									xmax+40, ymax+100, zmax+40 };

  	glEnableClientState(GL_VERTEX_ARRAY);
  	glVertexPointer(3, GL_FLOAT,0, wallArray);
 	glDrawArrays(GL_QUADS, 0, 24);
 	glDisableClientState(GL_VERTEX_ARRAY);
 */
 glPushMatrix();
  glBegin(GL_POLYGON);
     glVertex3f(xmin-20, ymax+100, zmin-20);
     glVertex3f(xmin-20, ymin-20, zmin-20);
     glVertex3f(xmax+20, ymin-20, zmin-20);
     glVertex3f(xmax+20, ymax+100, zmin-20);
  glEnd();

  glBegin(GL_POLYGON);
     glVertex3f(xmin-40, ymax+100, zmin-40);
     glVertex3f(xmin-40, ymin-40, zmin-40);
     glVertex3f(xmax+40, ymin-40, zmin-40);
     glVertex3f(xmax+40, ymax+100, zmin-40);
  glEnd();

  // Left
   glBegin(GL_POLYGON);
     glVertex3f(xmin-20, ymax+100, zmax+20);
     glVertex3f(xmin-20, ymin-20, zmax+20);
     glVertex3f(xmin-20, ymin-20, zmin-20);
     glVertex3f(xmin-20, ymax+100, zmin-20);
   glEnd();

   glBegin(GL_POLYGON);
     glVertex3f(xmin-40, ymax+100, zmax+40);
     glVertex3f(xmin-40, ymin-40, zmax+40);
     glVertex3f(xmin-40, ymin-40, zmin-40);
     glVertex3f(xmin-40, ymax+100, zmin-40);
   glEnd();

   // Right
   glBegin(GL_POLYGON);
     glVertex3f(xmax+20, ymax+100, zmin-20);
     glVertex3f(xmax+20, ymin-20, zmin-20);
     glVertex3f(xmax+20, ymin-20, zmax+20);
     glVertex3f(xmax+20, ymax+100, zmax+20);
   glEnd();

   glBegin(GL_POLYGON);
     glVertex3f(xmax+40, ymax+100, zmin-40);
     glVertex3f(xmax+40, ymin-40, zmin-40);
     glVertex3f(xmax+40, ymin-40, zmax+40);
     glVertex3f(xmax+40, ymax+100, zmax+40);
   glEnd();
   glPopMatrix();
   glEndList();
   madeWallList = 1;

  }
  if(renderWalls)
  glCallList(wallList);

	stopWallShaders();
 glPopMatrix();
 glShadeModel(GL_SMOOTH);





  /* Surface */
	static int surfaceList = 0;
	if(!surfaceList) {
		surfaceList = glGenLists(1);
		glNewList(surfaceList, GL_COMPILE);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, surfaceamb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, surfacedif);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, surfacespec);
  glMaterialf(GL_FRONT, GL_SHININESS, 50);

  glMap2f(GL_MAP2_VERTEX_3,
	  0, 1, 3, 5,
	  0, 1, 15, 5,
	  &surfacepoints[0][0][0]);
  glEnable(GL_MAP2_VERTEX_3);
  glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);

  glPushMatrix();
  glTranslatef(0, ymax, 0);
  glEvalMesh2(GL_FILL, 0, 20, 0, 20);
  glPopMatrix();


  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
	glEndList();
	}
	glCallList(surfaceList);

  for(i=0;i<5;i++){
    for(j=0;j<5;j++){
      //surfacepoints[i][j][1] += (sin(surfaceangles[i][j] * degrad))/(2+(rand()%2));
      surfacepoints[i][j][1] = 15*(sin(surfaceangles[i][j] * degrad));
      surfaceangles[i][j] += 2;
      surfaceangles[i][j] %= 360;
    }
  }

	}
   glPopMatrix ();

   if(!picking) { //extra stuff that doesn't need to be rendered when picking

//call the update for the collision detection
   cm->updateDistances();
#ifndef WIN32
   errorMess.render();
   _labels->Render();
#endif
   fishstats.render();
#ifndef WIN32
   glutSwapBuffers();
#else
	SwapBuffers(GetDC(hWnd));
#endif
   }
   com.possibleAddNode();
  // glFlush ();
#ifndef WIN32
	usleep(SleepTime);
#else
   Sleep(10);
   //Sleep(SleepTime/1000);
#endif
   //sleep(1);
} //end of render

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   //was 1500.0 and 2600.0
   gluPerspective(fov, (GLfloat) w/(GLfloat) h, 40, 5000.0);
   //glOrtho(-500, 500, -500, 500, 0, 1000);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}
#ifndef WIN32
void mouse(int button, int state, int x, int y)
{
   switch (button) {
      case GLUT_LEFT_BUTTON:
         if (state == GLUT_DOWN) {
	   //spin = (spin + 30) % 360;
	   //glRotated(3, 0, 1, 0);
         //   glutPostRedisplay();
         begin_picking(x, y);
         }
         break;
      case GLUT_RIGHT_BUTTON:
      	if(state == GLUT_DOWN) {
//      		glRotated(-3, 0, 1, 0);
//      		glutPostRedisplay();
      	}
         break;
      default:
         break;
   }
}
#endif

void fishShutDown() {

	glDeleteTextures(1, &texName);
	glDeleteLists(wallList, 1);
	glDeleteLists(bubbleList, 1);
	glDeleteLists(sandList, 1);
	shaderShutdown();
	//glDeleteLists(sandList2, 1);
	int i = 0;
	//for(i = 0; i < FISHCOUNT; i++)
		//free(fish[i].num);
#ifndef WIN32
	font.clean();
#endif
	ShutDown_castle();
	ShutDown_lighthouse();

	ShutDown_fish();

#ifndef WIN32
	glutDestroyWindow(window);
#endif
	exit(0);
}
#ifndef WIN32
void specialKeyboard(int key, int x, int y) {


	switch(key) {

		case GLUT_KEY_UP:
			moveCamera(2);
			break;

		case GLUT_KEY_DOWN:
			moveCamera(1);
			break;

		default:
			break;


	}


	return;


}
void keyboard(unsigned char key, int x, int y)
{

   switch (key) {
   	  case 32:
   	  	moveCamera(3);
   	  	break;
   	  case 119:  //w
   	  	//moveCamera(2);
   	  	break;
   	  case 115:  //s
   	  	//moveCamera(1);
   	  	break;
      case 27:
         fishShutDown();
         break;
     case 65:
//   		gluLookAt(0.0, 2000.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
   		break;
   	case 48:
   		picking = true;
   		glDisable(GL_LIGHTING);
   		glDisable(GL_FOG);
   		break;
   	case 49:
   		picking = false;
   		glEnable(GL_LIGHTING);
   		glEnable(GL_FOG);
   		break;
   }
}
#else
void SetupPixelFormat(HDC hDC)
{
    int pixelFormat;

    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),  // size
            1,                          // version
            PFD_SUPPORT_OPENGL |        // OpenGL window
            PFD_DRAW_TO_WINDOW |        // render to window
            PFD_DOUBLEBUFFER,           // support double-buffering
            PFD_TYPE_RGBA,              // color type
            32,                         // prefered color depth
            0, 0, 0, 0, 0, 0,           // color bits (ignored)
            0,                          // no alpha buffer
            0,                          // alpha bits (ignored)
            0,                          // no accumulation buffer
            0, 0, 0, 0,                 // accum bits (ignored)
            16,                         // depth buffer
            0,                          // no stencil buffer
            0,                          // no auxiliary buffers
            PFD_MAIN_PLANE,             // main layer
            0,                          // reserved
            0, 0, 0,                    // no layer, visible, damage masks
    };

    pixelFormat = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pixelFormat, &pfd);
}

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    static HGLRC hRC;
    int height, width;
	static int a = 0;
	HDC hDC;

    // dispatch messages
    switch (uMsg)
    {
    case WM_CREATE:         // window creation
	//	if(fishSplash && hWnd != fishSplash->getSplashHwnd()) {
        hDC = GetDC(hWnd);
        SetupPixelFormat(hDC);
        //SetupPalette();
        hRC = wglCreateContext(hDC);
        wglMakeCurrent(hDC, hRC);
	//	}
        break;

    case WM_DESTROY:            // window destroy
    case WM_QUIT:
    case WM_CLOSE:                  // windows is closing

        // deselect rendering context and delete it
        wglMakeCurrent(hDC, NULL);
        wglDeleteContext(hRC);

        // send WM_QUIT to message queue
	//	if(fishSplash && hWnd != fishSplash->getSplashHwnd())
			PostQuitMessage(0);
        break;

    case WM_SIZE:
        height = HIWORD(lParam);        // retrieve width and height
        width = LOWORD(lParam);

        reshape (width, height);

        break;

    case WM_ACTIVATEAPP:        // activate app
        break;

    case WM_PAINT:              // paint
//		if(a)
//			display();
//		else
//			a++;
//		SwapBuffers(hDC);
//		ValidateRect(hWnd, 0);


        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;


		case WM_LBUTTONDOWN:		// left mouse button
     // LOWORD(lParam);    // This gives us the horizontal (the X position) of
      begin_picking( LOWORD(lParam), HIWORD(lParam));                                // the cursor
    //  Base.y_cursor = HIWORD(lParam);

	break;
		case WM_KEYDOWN:
        int fwKeys;
        LPARAM keyData;
        fwKeys = (int)wParam;    // virtual-key code
        keyData = lParam;          // key data

        switch(wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
		}
	default:
		break;

	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


DWORD WINAPI RunThread(LPVOID ptr) {
	reinterpret_cast<Runnable*>(ptr)->Start();
	return NULL;
}

#endif

#ifndef WIN32
int main(int argc, char** argv) {
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    WNDCLASSEX windowClass;     // window class
    HWND       hwnd;            // window handle
    MSG        msg;             // message
    DWORD      dwExStyle;       // Window Extended Style
    DWORD      dwStyle;         // Window Style
    RECT       windowRect;
	BOOL	   fullscreen;
//	HDC		   hDC;
	BOOL	   exiting;





    windowRect.left=(long)0;                        // Set Left Value To 0
    windowRect.right= 1024; // Set Right Value To Requested Width
    windowRect.top=(long)0;                         // Set Top Value To 0
    windowRect.bottom= 768;   // Set Bottom Value To Requested Height

    // fill out the window class structure
    windowClass.cbSize          = sizeof(WNDCLASSEX);
    windowClass.style           = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc     = MainWindowProc;
    windowClass.cbClsExtra      = 0;
    windowClass.cbWndExtra      = 0;
    windowClass.hInstance       = hInstance;
    windowClass.hIcon           = 0;//LoadIcon(hInstance, MAKEINTRESOURCE(ID_ICON1));  // default icon
    windowClass.hCursor         = LoadCursor(hInstance, IDC_ARROW);//LoadCursor(hInstance, MAKEINTRESOURCE(ID_CURSOR1));      // default arrow
    windowClass.hbrBackground   = NULL;                             // don't need background
    windowClass.lpszMenuName    = NULL;                             // no menu
    windowClass.lpszClassName   = "GLClass";
    windowClass.hIconSm         = LoadIcon(NULL, IDI_WINLOGO);      // windows logo small icon
#endif
	fishSplash = 0;
	picking = false;
  int i;
  	parse_config();
  	//intialize random seed
	srand( (unsigned) time( 0 ));

  for(i=0;i<FISHCOUNT;i++){
 // 	fish[i].num = (char*)malloc(sizeof(char)*4);
    fish[i].location[0] = (rand() % (int)(1.8*xmax))-xmax;
    fish[i].location[1] = (rand() % (int)(1.8*ymax))-(0.9*ymax) + 30;
    fish[i].location[2] = (rand() % (int)(1.8*zmax))-zmax;
    fish[i].homedepth = fish[i].location[1];
    fish[i].speed = 1;
    fish[i].rotation = rand() % 360;
    fish[i].finrotation = (rand() % 6)-3;
    fish[i].size = ((float)(rand()%220))/100.0;
    fish[i].targetsize = fish[i].size;
    fish[i].alive = 1;
    fish[i].dying = 0;
    fish[i].undying = 0;
    fish[i].deathangle = 0;
    fish[i].growing = 0;
    fish[i].shrinking = 0;
    fish[i].blinking = EYE_OPEN;
    fish[i].immune = 0;
    fish[i].targetrotation = fish[i].rotation;
    fish[i].rotationspeed = 1;
    fish[i].idnumber[0] = (int)fmod((double)i,10);
    fish[i].idnumber[1] = (int)(fmod((double)i,100)/10);
    if(i > 99)
  	 	fish[i].idnumber[2] = 1;
  	 else
  	 	fish[i].idnumber[2] = 0;
  	 fish[i].deadcounter = 0;

	  fish[i].bobcounter = rand() % 360;

    //fish[i].finrotation = 0;
    fish[i].oldfinrotation[0] = 0;
    fish[i].oldfinrotation[1] = 0;
    fish[i].oldfinrotation[2] = 0;

    if(rand()%2)
      fish[i].findirection = 1;
    else
      fish[i].findirection = -1;

    if(rand()%2)
      fish[i].direction = 1;
    else
      fish[i].direction = -1;
  }

#ifdef WIN32
	 if (!RegisterClassEx(&windowClass))

        return 0;
	fullscreen = FALSE;


    if (fullscreen)                             // fullscreen?
    {
        DEVMODE dmScreenSettings;                   // device mode
        memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = 1024;         // screen width
        dmScreenSettings.dmPelsHeight = 768;           // screen height
        dmScreenSettings.dmBitsPerPel = 32;             // bits per pixel
        dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

        //
        if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        {
            // setting display mode failed, switch to windowed
            MessageBox(NULL, "Display mode failed", NULL, MB_OK);
            fullscreen = FALSE;
        }
    }

    if (fullscreen)                             // Are We Still In Fullscreen Mode?
    {
        dwExStyle=WS_EX_APPWINDOW;                  // Window Extended Style
        dwStyle=WS_POPUP;                       // Windows Style
        ShowCursor(FALSE);                      // Hide Mouse Pointer
    }
    else
    {
        dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;   // Window Extended Style
        dwStyle=WS_OVERLAPPEDWINDOW;                    // Windows Style
    }

    AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);     // Adjust Window To True Requested Size

    // class registered, so now create our window
    hwnd = CreateWindowEx(NULL,                                 // extended style
        "GLClass",                          // class name
        "Fish",      // app name
        dwStyle | WS_CLIPCHILDREN |
        WS_CLIPSIBLINGS,
        0, 0,                               // x,y coordinate
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top, // width, height
        NULL,                               // handle to parent
        NULL,                               // handle to menu
        hInstance,                          // application instance
        NULL);                              // no extra params


	hWnd = hwnd;
    HDC hDC = GetDC(hwnd);

    // check if window creation failed (hwnd would equal NULL)
    if (!hwnd)
        return 0;
	ShowCursor(TRUE);



	exiting = FALSE;


#endif //WIN32





  bubblerloc[0] = xmin/2;
  bubblerloc[1] = ymin;
  bubblerloc[2] = 0;
	//this stuff should be in init
  for(i=0;i<BUBBLECOUNT;i++){
      bubbles[i].location[0] = bubblerloc[0] + ((rand() % 101)-50);
      bubbles[i].location[1] = bubblerloc[1];
      bubbles[i].location[2] = bubblerloc[2] + ((rand() % 101)-50);
      bubbles[i].speed = (float)((rand() % 100) + 50)/50;
  }
#ifdef WIN32
  fishSplash = new FishSplash("GLClass", hInstance);
  fishSplash->Display();
#endif
  read_file();
#ifdef WIN32
  fishSplash->Close();
#endif

  //initialize to eliminate bad output
  updateTime();

#ifndef WIN32
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(winx, winy);
   glutInitWindowPosition(0, 0);


   //Just a note, DO NOT USE glutEnterGameMode()  it jacks the comp
   window = glutCreateWindow("Fishies!");
	//glutFullScreen();
	#endif
   init();
#ifndef	WIN32
   signal(SIGALRM, &alarm_handler);
  // fps meter
   alarm(1);
   //turn on cursor so that you can click on the fish
  // glutSetCursor(GLUT_CURSOR_NONE);
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutMouseFunc(mouse);
   glutKeyboardFunc(keyboard);
   glutSpecialFunc(specialKeyboard);
   glutIdleFunc(display);
   fflush(stdout);
   glutMainLoop();

#else
       ShowWindow(hwnd, SW_SHOW);          // display the window
	UpdateWindow(hwnd);
   while (!exiting)
    {


		display();

        SwapBuffers(hDC);

        while (PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (!GetMessage (&msg, NULL, 0, 0))
            {
                exiting = TRUE;
                break;
            }

            TranslateMessage (&msg);
            DispatchMessage (&msg);
        }
    }


    if (fullscreen)
    {
        ChangeDisplaySettings(NULL,0);          // If So Switch Back To The Desktop
        ShowCursor(TRUE);                       // Show Mouse Pointer
    }
	fishShutDown();
#endif
   printf("test before return\n");
   fflush(stdout);
   return 0;
}
