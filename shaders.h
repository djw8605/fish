
#ifndef SHADERS_H
#define SHADERS_H

#include <GL/glut.h>
#include <GL/glx.h>

struct shaderStruct {
	GLhandleARB vertex;
	GLhandleARB fragment;
	GLhandleARB program;
};

void createShaders();
void shaderShutdown();
int startFishShaders();
int stopFishShaders();
int startBubbleShaders();
int stopBubbleShaders();
int startWallShaders();
int stopWallShaders();
int pushShader();
int popShader();
int nightShaders();





#endif
