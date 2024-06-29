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
	unsigned char useLegacy;
} InitParams;

typedef struct GameEngine {
	SDL_Window* window;
	SDL_Renderer* renderer; // only by calling init2D
	float pixelScale;
	int width;
	int height;
} GameEngine;

// Initialize an SDL window and the builtin SDL renderer,
// which is a 2D-only texture based hardware accelerated renderer.
GameEngine initSDLEngine(InitParams params);

// Initialize an SDL window and GLAD and an OpenGL context
GameEngine initGLEngine(InitParams params);

// Initialize an SDL window and Vulkan
GameEngine initVulkanEngine(InitParams params);

void dealloc(GameEngine* engine);

float setRendererPixelScale(SDL_Window* window, SDL_Renderer *renderer);

void setFullscreenGL(GameEngine *engine, float *aspect);

void glDebugInfo();

#ifdef __cplusplus
}
#endif
