#pragma once

#include "glad.h"
#ifdef _WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct InitParams {
	unsigned int flags;
	const char* title;
	int width;
	int height;
	unsigned char disableShaders;
} InitParams;

typedef struct GameEngine {
	SDL_Window* window;
	SDL_Renderer* renderer; // only by calling init2D
	float pixelScale;
	int width;
	int height;
} GameEngine;

// Initialize an SDL window and renderer for a fixed-function
// pipeline OpenGL renderer.
GameEngine init2D(InitParams params);

// Initialize GLEW and an SDL window for a programmable pipeline
// using OpenGL making accessible shaders.
GameEngine init3D(InitParams params);

void dealloc(GameEngine* engine);

float setRendererPixelScale(SDL_Window* window, SDL_Renderer *renderer);

void setFullscreenGL(GameEngine *engine, float *aspect);

void viewportTest(GameEngine *engine, SDL_Texture *texture);

void glDebugInfo();

// void setFullScreen(GameEngine *engine);
// void drawLoop(GameEngine* engine);

#ifdef __cplusplus
}
#endif
