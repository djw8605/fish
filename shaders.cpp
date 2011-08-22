#include <GL/glut.h>
#include <GL/glx.h>
#include <stdio.h>
#include "fishmodel.h"
#include <iostream>
#include "shaders.h"
#include <fstream>
#include <sstream>
#include <string.h>
#include <cmath>

using namespace std;
short int useShaders;
//#define glXGetProcAddress(x) (*glXGetProcAddressARB)((const GLubyte*)x)

shaderStruct fishShader;
shaderStruct bubbleShader;
shaderStruct wallShader;

#define USE_SHADERS
//#define SHADER_DEBUG

char* readTextFromFile(const char* filename) {
	
	ifstream file(filename); 
	string contents;
	string final_contents;
	while(file>>contents) { 
		final_contents.append(contents.c_str());
		final_contents.append(" ");
	}
	//printf("file contents:\n%s\n",final_contents.c_str());
	
	int content_length = final_contents.length();
	char* stuff = new char[content_length+1];
	strcpy(stuff, final_contents.c_str());
	return stuff;
	
	
	
	
}




static PFNGLCREATEPROGRAMOBJECTARBPROC     glCreateProgramObjectARB = NULL;
static PFNGLCREATESHADEROBJECTARBPROC      glCreateShaderObjectARB = NULL;
static PFNGLSHADERSOURCEARBPROC            glShaderSourceARB = NULL;
static PFNGLCOMPILESHADERARBPROC           glCompileShaderARB = NULL;
static PFNGLGETOBJECTPARAMETERIVARBPROC    glGetObjectParameterivARB = NULL;
static PFNGLATTACHOBJECTARBPROC            glAttachObjectARB = NULL;
static PFNGLGETINFOLOGARBPROC              glGetInfoLogARB = NULL;
static PFNGLLINKPROGRAMARBPROC             glLinkProgramARB = NULL;
static PFNGLUSEPROGRAMOBJECTARBPROC        glUseProgramObjectARB = NULL;
static PFNGLGETUNIFORMLOCATIONARBPROC      glGetUniformLocationARB = NULL;
//static PFNGLUNIFORM1FARBPROC               glUniform1f = NULL;
#ifndef __APPLE__
static PFNGLUNIFORM1IARBPROC               glUniform1i = NULL;
#endif
static PFNGLDETACHOBJECTARBPROC			   glDetachObjectARB = NULL;
static PFNGLDELETEOBJECTARBPROC			   glDeleteObjectARB = NULL;
static PFNGLGETHANDLEARBPROC			   glGetHandleARB = NULL;


void deleteShader(shaderStruct* toDelete) {
	glDetachObjectARB(toDelete->program, toDelete->vertex);
	glDetachObjectARB(toDelete->program, toDelete->fragment);
	glDeleteObjectARB(toDelete->vertex);
	glDeleteObjectARB(toDelete->fragment);
	glDeleteObjectARB(toDelete->program);
	
	
	
}

void shaderShutdown() {
	if(!useShaders)
		return;
	
	deleteShader(&fishShader);
	deleteShader(&bubbleShader);
	deleteShader(&wallShader);
	glUseProgramObjectARB(0);
	//printf("deleting stuff");
	
	
	
	
	//glDeleteObjectARB(shaderRunning.program);
	
	
	
}


void ReadShaderErrors(GLhandleARB program)
{
#ifdef SHADER_DEBUG
	char buf[200];
		int length;
	glGetInfoLogARB(program,200,&length,buf);
	if(length)
		printf("received %i chars, %s\n",length,buf);
#endif
}

static void createSingleShader(char* vertexShader, char* fragmentShader, shaderStruct* shader) {
	
	shader->program = glCreateProgramObjectARB();
	char *vs,*fs;
	char buf[200];
	int length;
	shader->vertex = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    shader->fragment = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

                //vs = textFileRead("toon.vert");
                //fs = textFileRead("toon.frag");
                //vs = "void main() { vec3 normal, lightDir; vec4 diffuse, ambient, globalAmbient; float NdotL;normal = normalize(gl_NormalMatrix * gl_Normal);lightDir = normalize(vec3(gl_LightSource[0].position));NdotL = max(dot(normal, lightDir), 0.0);diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse; ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient; globalAmbient = gl_LightModel.ambient * gl_FrontMaterial.ambient; gl_FrontColor =  NdotL * diffuse + globalAmbient + ambient;gl_Position = ftransform();}";
                //fs = "void main(){gl_FragColor = gl_Color;}";
                vs = readTextFromFile(vertexShader);
				fs = readTextFromFile(fragmentShader);
				const char * vv = vs;
                const char * ff = fs;
			
                glShaderSourceARB(shader->vertex, 1, &vv,NULL);
                glShaderSourceARB(shader->fragment, 1, &ff,NULL);
				delete vs; delete fs;
                //free(vs);free(fs);
			glCompileShaderARB(shader->vertex);

	ReadShaderErrors(shader->program);

                glCompileShaderARB(shader->fragment);
		
    ReadShaderErrors(shader->program);

          


 glAttachObjectARB(shader->program,shader->vertex);

 ReadShaderErrors(shader->program);
 glAttachObjectARB(shader->program,shader->fragment);
		
 ReadShaderErrors(shader->program);

	

	glLinkProgramARB(shader->program);


	ReadShaderErrors(shader->program);
	
	
	
	
}


void createShaders() {
        useShaders = 0;
#ifndef USE_SHADERS
	return;
#endif
        if(!IsExtensionSupported((char*)"GL_ARB_fragment_shader") && !IsExtensionSupported((char*)"GL_ARB_vertex_shader"))
        {
                printf("Your computer doesn't allow fragment shaders and/or vertex shaders, not shading\n");
                useShaders = 0;
                return;
        }
        useShaders = 1;



	//get all the function stuff

	readTextFromFile("shaders.cpp");
glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)
        glXGetProcAddress((const GLubyte*)"glCreateProgramObjectARB");
    glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)
        glXGetProcAddress((const GLubyte*)"glCreateShaderObjectARB");
    glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)
        glXGetProcAddress((const GLubyte*)"glShaderSourceARB");
    glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)
        glXGetProcAddress((const GLubyte*)"glCompileShaderARB");
    glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)
        glXGetProcAddress((const GLubyte*)"glGetObjectParameterivARB");
    glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)
        glXGetProcAddress((const GLubyte*)"glAttachObjectARB");
    glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)
        glXGetProcAddress((const GLubyte*)"glGetInfoLogARB");
    glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)
        glXGetProcAddress((const GLubyte*)"glLinkProgramARB");
    glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)
        glXGetProcAddress((const GLubyte*)"glUseProgramObjectARB");
    glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)
        glXGetProcAddress((const GLubyte*)"glGetUniformLocationARB");
//    glUniform1f = (PFNGLUNIFORM1FARBPROC)
//        glXGetProcAddress((const GLubyte*)"glUniform1fARB");
#ifndef __APPLE__
    glUniform1i = (PFNGLUNIFORM1IARBPROC)
        glXGetProcAddress((const GLubyte*)"glUniform1iARB");
#endif
	glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)
		glXGetProcAddress((const GLubyte*)"glDetachObjectARB");
	glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)
		glXGetProcAddress((const GLubyte*)"glDeleteObjectARB");
	glGetHandleARB = (PFNGLGETHANDLEARBPROC)
		glXGetProcAddress((const GLubyte*)"glGetHandleARB");
	
	if(!glDetachObjectARB)
		printf("unable to get detach function");
	
	//initialize the program
	
	
	
		

	
	createSingleShader((char*)"fishvertex.shader", (char*)"fishfragment.shader", &fishShader);
	createSingleShader((char*)"bubblevertex.shader", (char*)"bubblefragment.shader", &bubbleShader);
	createSingleShader((char*)"glassvertex.shader", (char*)"glassfragment.shader", &wallShader);
	


}

int nightShaders() {
	
	char buf[200];
	int length;
	static int night = 0;
	if (night == 0)
		night = 1;
	else
		night = 0;
	
	glUseProgramObjectARB(fishShader.program);
	GLint uniformLocation = glGetUniformLocationARB(fishShader.program, /*(const GLcharARB*)*/"night");
	ReadShaderErrors(fishShader.program);


	glUniform1i(uniformLocation, (GLint) night);

	ReadShaderErrors(fishShader.program);
	glUseProgramObjectARB(0);
  
	
}


int startFishShaders() {
	//first, error checking, make sure no other shader is running

	if(!useShaders)
		return 0;
	

	
	glUseProgramObjectARB(fishShader.program);
	//glUniform1f(uniformLocation, counter);

	ReadShaderErrors(fishShader.program);

  }

int stopFishShaders() {
	

		//stop the current shader

		//glDetachObjectARB(shaderRunning.program, shaderRunning.vertex);
		//glDetachObjectARB(shaderRunning.program, shaderRunning.fragment);
		//glDeleteObjectARB(shaderRunning.vertex);
		//glDeleteObjectARB(shaderRunning.fragment);
	//	glDeleteObjectARB(shaderRunning.program);
	//	glLinkProgramARB(shaderRunning.program);
	if(!useShaders)
		return 0;
		glUseProgramObjectARB(0);
	
		ReadShaderErrors(fishShader.program);


	
	
}

int startBubbleShaders() {
	
	if(!useShaders)
		return 0;
	glUseProgramObjectARB(bubbleShader.program);
	ReadShaderErrors(bubbleShader.program);
	
}


int stopBubbleShaders() {
	if(!useShaders)
		return 0;
	glUseProgramObjectARB(0);
}

int startWallShaders() {
	
	if(!useShaders)
		return 0;
	
	glUseProgramObjectARB(wallShader.program);
	
}
int stopWallShaders(){
	if(!useShaders)
		return 0;

	glUseProgramObjectARB(0);

}


struct shaderNode {
	GLhandleARB shader;
	shaderNode* next;
};

static shaderNode* shaderStack = 0;

int pushShader() {

	/* if using sharders */
	if(!useShaders)
		return 0;

	/* Get the current running handle, do nothing if nothing is running */
	GLhandleARB tempHandle = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
	if(!tempHandle)
		return 0;
	
	shaderNode* temp = new shaderNode;
	temp->shader = tempHandle;
	temp->next = shaderStack;
	shaderStack = temp;
	glUseProgramObjectARB(0);
	
	
}

int popShader() {
	
	/* if we're using shaders and there's something on the stack */
	if(!useShaders || !shaderStack)
		return 0;

	/* pop the shader off the stack */
	shaderNode* temp = shaderStack;
	shaderStack = shaderStack->next;
	delete temp;
	
	/* if there's anything left on the stack */
	if(!shaderStack)
		glUseProgramObjectARB(0);
	else
		glUseProgramObjectARB(shaderStack->shader);

}






