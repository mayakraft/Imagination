#ifndef game_engine_h
#define game_engine_h

#include <stdexcept>
#include <GL/glew.h>
#ifdef _WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#endif

// #include <OpenGL/gl.h> // gl3.h
// #include <SDL2/SDL_opengl.h>

struct InitParams {
	unsigned int flags;
	const char* title;
	int width;
	int height;
	bool disableShaders;
};

struct GameEngine {
	SDL_Window* window;
	SDL_Renderer* renderer; // only by calling init2D
	float pixelScale;
};

float setRendererPixelScale(SDL_Window* window, SDL_Renderer *renderer) {
	int winW, winH, renW, renH;
	SDL_GetWindowSize(window, &winW, &winH);
	SDL_GetRendererOutputSize(renderer, &renW, &renH);
	if (renW == winW) { return 1.0; }
	float widthScale = (float)renW / (float) winW;
	float heightScale = (float)renH / (float) winH;
	SDL_RenderSetScale(renderer, widthScale, heightScale);
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

// Initialize an SDL window and renderer for a fixed-function
// pipeline OpenGL renderer.
GameEngine init2D(InitParams params) {
	if (SDL_Init(params.flags) < 0) {
		throw std::runtime_error(SDL_GetError());
	}

	SDL_Window *window = SDL_CreateWindow(params.title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		params.width,
		params.height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
	// SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN
	if (window == NULL) { throw std::runtime_error(SDL_GetError()); }

	SDL_Renderer *renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) { throw std::runtime_error(SDL_GetError()); }

	float pixelScale = setRendererPixelScale(window, renderer);

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		throw std::runtime_error(IMG_GetError());
	}

	return GameEngine {
		.window = window,
		.renderer = renderer,
		.pixelScale = pixelScale,
	};
}

// Initialize GLEW and an SDL window for a programmable pipeline
// using OpenGL making accessible shaders.
GameEngine init3D(InitParams params) {
	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
	if (SDL_Init(params.flags) < 0) {
		throw std::runtime_error(SDL_GetError());
	}

	if (params.disableShaders) {
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
	if (window == NULL) { throw std::runtime_error(SDL_GetError()); }

	// On MacOS, this needs to be called immediately after SDL_CreateWindow,
	// if not, any subsequent SDL method call involving window will somehow remove
	// the "SDL_WINDOW_OPENGL" flat and replace it with Metal, or something.
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == NULL) { throw std::runtime_error(SDL_GetError()); }

	int getW, getH;
	int getGLW, getGLH;
	SDL_GetWindowSize(window, &getW, &getH);
	SDL_GL_GetDrawableSize(window, &getGLW, &getGLH);
	// printf("SDL_GetWindowSize %d, %d\n", getW, getH);
	// printf("SDL_GL_GetDrawableSize %d, %d\n", getGLW, getGLH);
	float pixelScale = (float)getGLW / (float)getW;

	// initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		const char* errorChar = reinterpret_cast<char const*>(glewGetErrorString(glewError));
		throw std::runtime_error(errorChar);
	}

	// use vsync
	if (SDL_GL_SetSwapInterval(1) < 0) {
		throw std::runtime_error(SDL_GetError());
	}

	// initialize SDL_image
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		throw std::runtime_error(IMG_GetError());
	}

	return GameEngine {
		.window = window,
		.pixelScale = pixelScale,
	};
}

// void setFullScreen(GameEngine *engine) {
// 	SDL_SetWindowFullscreen(engine->window, SDL_WINDOW_FULLSCREEN);
// }

SDL_Texture* loadTexture(GameEngine* engine, const char* path) {
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
	} else {
		// Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(engine->renderer, loadedSurface);
		if (newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		}
		// Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
	return newTexture;
}

void dealloc(GameEngine* engine) {
	if (engine->renderer != NULL) SDL_DestroyRenderer(engine->renderer);
	if (engine->window != NULL) SDL_DestroyWindow(engine->window);
	engine->renderer = NULL;
	engine->window = NULL;
	IMG_Quit();
	SDL_Quit();
	// glDeleteProgram(gProgramID);
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

void viewportTest(GameEngine *engine, SDL_Texture *texture) {
	float SCREEN_WIDTH = 640;
	float SCREEN_HEIGHT = 480;
	//Top left corner viewport
	SDL_Rect topLeftViewport;
	topLeftViewport.x = 0;
	topLeftViewport.y = 0;
	topLeftViewport.w = (int)SCREEN_WIDTH / 2;
	topLeftViewport.h = (int)SCREEN_HEIGHT / 2;
	SDL_RenderSetViewport( engine->renderer, &topLeftViewport );
	//Render texture to screen
	SDL_RenderCopy( engine->renderer, texture, NULL, NULL );
	//Bottom viewport
	SDL_Rect bottomViewport;
	bottomViewport.x = 0;
	bottomViewport.y = (int)SCREEN_HEIGHT / 2;
	bottomViewport.w = SCREEN_WIDTH;
	bottomViewport.h = (int)SCREEN_HEIGHT / 2;
	SDL_RenderSetViewport( engine->renderer, &bottomViewport );
	//Render texture to screen
	SDL_RenderCopy( engine->renderer, texture, NULL, NULL );
	//Update screen
	SDL_RenderPresent( engine->renderer );
}

#endif
