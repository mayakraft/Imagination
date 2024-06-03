#ifndef camera_h
#define camera_h

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

enum { LEFT, RIGHT };
unsigned char HANDED = LEFT; // 0:left, 1:right. flip coordinate axes orientation.

enum{  FPP,  POLAR,  ORTHO  }; // first persion, polar, orthographic
static unsigned char PERSPECTIVE = FPP;  // initialize point of view in this state
static int WIDTH = 800;  // (readonly) adjust window dimensions. not able to be set at runtime
static int HEIGHT = 600; // (readonly)
static unsigned char FULLSCREEN = 0;  // fullscreen:1   window:0
static float ASPECT;  // (readonly) aspect ratio of window dimensions
// PROJECTION
static float NEAR_CLIP = 0.1;
static float FAR_CLIP = 10000.0;
static float FOV = 0.1;
// CAMERA ORIENTATION AND POSITION
float ORIGIN[3] = {0.0f, 0.0f, 0.0f};  // x, y, z, location of the eye
float HORIZON[3] = {0.0f, 0.0f, 7.0f};   // azimuth, altitude, zoom (log)
float WINDOW[4]; // x, y, width, height
static float EYE_HEIGHT = 1.0;  // camera offset above the origin ground plane in first-person perspective mode.

static float _invert_y_m[16] = {1,0,0,0,0,-1,0,0,0,0,1,0,0,0,0,1};

void firstPersonPerspective(){
	PERSPECTIVE = FPP;
	float a = (float)fmin(WIDTH, HEIGHT) / fmax(WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(WIDTH < HEIGHT){ glFrustum(-FOV, FOV, -FOV/a, FOV/a, NEAR_CLIP, FAR_CLIP); }
	else              { glFrustum(-FOV/a, FOV/a, -FOV, FOV, NEAR_CLIP, FAR_CLIP); }
	// change POV
	glRotatef(-90-HORIZON[1], 1, 0, 0);
	glRotatef(90+HORIZON[0], 0, 0, 1);
	// for left handed, invert the Y
	if(!HANDED){ glMultMatrixf(_invert_y_m); }
	// raise POV 1.0 above the floor, 1.0 is an arbitrary value
	glTranslatef(0.0f, 0.0f, -EYE_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
}

void polarPerspective(){
	PERSPECTIVE = POLAR;
	float a = (float)fmin(WIDTH, HEIGHT) / fmax(WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(WIDTH < HEIGHT){ glFrustum(-FOV, FOV, -FOV/a, FOV/a, NEAR_CLIP, FAR_CLIP); }
	else              { glFrustum(-FOV/a, FOV/a, -FOV, FOV, NEAR_CLIP, FAR_CLIP); }
	// change POV
	glTranslatef(0, 0, -HORIZON[2]);
	glRotatef(-90+HORIZON[1], 1, 0, 0);
	glRotatef(90+180+HORIZON[0], 0, 0, 1);
	// for left handed, invert the Y
	if(!HANDED){ glMultMatrixf(_invert_y_m); }
	glMatrixMode(GL_MODELVIEW);
}

void orthoPerspective(float x, float y, float width, float height){
	PERSPECTIVE = ORTHO;
	WINDOW[0] = x;
	WINDOW[1] = y;
	WINDOW[2] = width;
	WINDOW[3] = height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	switch(HANDED){
		case LEFT: glOrtho(x, width + x, height + y, y, -FAR_CLIP, FAR_CLIP); break;
		case RIGHT: glOrtho(x, width + x, y, height + y, -FAR_CLIP, FAR_CLIP); break;
	}
	glMatrixMode(GL_MODELVIEW);
}

#endif
