#include "initialize.h"
#include "../vulkan/initialize.h"
#include "SDL2/SDL_vulkan.h"

float getRendererPixelScale(SDL_Window* window, SDL_Renderer *renderer) {
	int winW, winH, renW, renH;
	SDL_GetWindowSize(window, &winW, &winH);
	SDL_GetRendererOutputSize(renderer, &renW, &renH);
	if (renW == winW) { return 1.0; }
	float widthScale = (float)renW / (float) winW;
	// it's actually not good to call SDL_RenderSetScale for an SDL Renderer,
	// it makes the window pixellated again (removes the retina scale).
	// SDL_RenderSetScale(renderer, widthScale, heightScale);
	// SDL_RenderSetLogicalSize(renderer, renW, renH);
	return widthScale;
}

void setFullscreenGL(GameEngine *engine, float *aspect) {
	SDL_SetWindowFullscreen(engine->window, SDL_WINDOW_FULLSCREEN);
	// SDL_SetWindowFullscreen(engine->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	int resizeW, resizeH;
	SDL_GetWindowSize(engine->window, &resizeW, &resizeH);
	glViewport(0, 0, resizeW * engine->pixelScale, resizeH * engine->pixelScale);
	if (aspect != NULL) { *aspect = (float)resizeW / resizeH; }
}

// void setFullScreen(GameEngine *engine) {
// 	SDL_SetWindowFullscreen(engine->window, SDL_WINDOW_FULLSCREEN);
// }

// Initialize an SDL window and renderer for a fixed-function
// pipeline OpenGL renderer.
GameEngine initSDLEngine(InitParams params) {
	if (SDL_Init(params.flags) < 0) { fputs(SDL_GetError(), stderr); }

	SDL_Window *window = SDL_CreateWindow(params.title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		params.width,
		params.height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
	// SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN
	if (window == NULL) { fputs(SDL_GetError(), stderr); }

	SDL_Renderer *renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) { fputs(SDL_GetError(), stderr); }

	float pixelScale = getRendererPixelScale(window, renderer);

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		fputs(IMG_GetError(), stderr);
	}

	GameEngine engine;
	engine.window = window;
	engine.renderer = renderer;
	engine.pixelScale = pixelScale;
	engine.width = (int)(params.width * pixelScale);
	engine.height = (int)(params.height * pixelScale);
	return engine;
}

// Initialize GLEW and an SDL window for a programmable pipeline
// using OpenGL making accessible shaders.
GameEngine initGLEngine(InitParams params) {
	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
	if (SDL_Init(params.flags) < 0) {
		fputs(SDL_GetError(), stderr);
	}

	SDL_GL_LoadLibrary(NULL);
	if (params.useLegacy) {
		// should we specify a version?
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	} else {
		// need at least OpenGL 3.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	}
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_Window *window = SDL_CreateWindow(params.title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		params.width,
		params.height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
	if (window == NULL) { fputs(SDL_GetError(), stderr); }

	// On MacOS, this needs to be called immediately after SDL_CreateWindow,
	// if not, any subsequent SDL method call involving window will somehow remove
	// the "SDL_WINDOW_OPENGL" flat and replace it with Metal, or something.
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == NULL) { fputs(SDL_GetError(), stderr); }

	int getW, getH;
	int getGLW, getGLH;
	SDL_GetWindowSize(window, &getW, &getH);
	SDL_GL_GetDrawableSize(window, &getGLW, &getGLH);
	// printf("SDL_GetWindowSize %d, %d\n", getW, getH);
	// printf("SDL_GL_GetDrawableSize %d, %d\n", getGLW, getGLH);
	float pixelScale = (float)getGLW / (float)getW;

	if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
		fputs("Failed to initialize OpenGL context\n", stderr);
	}
	// printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	// use vsync
	if (SDL_GL_SetSwapInterval(1) < 0) {
		fputs(SDL_GetError(), stderr);
	}

	// initialize SDL_image
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		fputs(IMG_GetError(), stderr);
	}

	GameEngine engine;
	engine.window = window;
	engine.pixelScale = pixelScale;
	engine.width = (int)(params.width * pixelScale);
	engine.height = (int)(params.height * pixelScale);
	return engine;
}

GameEngine initVulkanEngine(InitParams params) {
	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
	if (SDL_Init(params.flags) < 0) {
		fputs(SDL_GetError(), stderr);
	}

	SDL_Window *window = SDL_CreateWindow(params.title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		params.width,
		params.height,
		SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI); // | SDL_WINDOW_SHOWN
	if (window == NULL) { fputs(SDL_GetError(), stderr); }

	int getW, getH;
	int getVKW, getVKH;
	SDL_GetWindowSize(window, &getW, &getH);
	SDL_Vulkan_GetDrawableSize(window, &getVKW, &getVKH);
	// printf("SDL_GetWindowSize %d, %d\n", getW, getH);
	// printf("SDL_Vulkan_GetDrawableSize %d, %d\n", getGLW, getGLH);
	float pixelScale = (float)getVKW / (float)getW;

	// initialize SDL_image
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		fputs(IMG_GetError(), stderr);
	}

	// initVulkan(&window, params.title);

	GameEngine engine;
	engine.window = window;
	engine.pixelScale = pixelScale;
	engine.width = (int)(params.width * pixelScale);
	engine.height = (int)(params.height * pixelScale);
	return engine;
}

void dealloc(GameEngine* engine) {
	if (engine->renderer != NULL) SDL_DestroyRenderer(engine->renderer);
	if (engine->window != NULL) SDL_DestroyWindow(engine->window);
	engine->renderer = NULL;
	engine->window = NULL;
	IMG_Quit();
	SDL_Quit();
}

void glDebugInfo() {
	printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
	printf("Vendor:   %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version:  %s\n", glGetString(GL_VERSION));
	printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

//void drawLoop(GameEngine* engine) {
//    // event will be filled with some kind of input event (if exists)
//    // https://wiki.libsdl.org/SDL2/SDL_Event
//    SDL_Event e;
//    bool quit = false;
//    while (!quit) {
//        while (SDL_PollEvent(&e)) {
//            if (e.type == SDL_QUIT) {
//                quit = true;
//            }
//        }
//        // update, draw
//        SDL_UpdateWindowSurface(engine->window);
//    }
//}

// void viewportTest(GameEngine *engine, SDL_Texture *texture) {
// 	float SCREEN_WIDTH = 640;
// 	float SCREEN_HEIGHT = 480;
// 	//Top left corner viewport
// 	SDL_Rect topLeftViewport;
// 	topLeftViewport.x = 0;
// 	topLeftViewport.y = 0;
// 	topLeftViewport.w = (int)SCREEN_WIDTH / 2;
// 	topLeftViewport.h = (int)SCREEN_HEIGHT / 2;
// 	SDL_RenderSetViewport( engine->renderer, &topLeftViewport );
// 	//Render texture to screen
// 	SDL_RenderCopy( engine->renderer, texture, NULL, NULL );
// 	//Bottom viewport
// 	SDL_Rect bottomViewport;
// 	bottomViewport.x = 0;
// 	bottomViewport.y = (int)SCREEN_HEIGHT / 2;
// 	bottomViewport.w = SCREEN_WIDTH;
// 	bottomViewport.h = (int)SCREEN_HEIGHT / 2;
// 	SDL_RenderSetViewport( engine->renderer, &bottomViewport );
// 	//Render texture to screen
// 	SDL_RenderCopy( engine->renderer, texture, NULL, NULL );
// 	//Update screen
// 	SDL_RenderPresent( engine->renderer );
// }
