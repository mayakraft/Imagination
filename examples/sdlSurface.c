#include <stdio.h>
#ifdef _WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#endif

SDL_Surface* loadSurface(SDL_Surface* surface, const char* path) {
	SDL_Surface* optimizedSurface = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
	} else {
		optimizedSurface = SDL_ConvertSurface(loadedSurface, surface->format, 0);
		if (optimizedSurface == NULL) {
			printf("Unable to optimize image %s! SDL Error: %s\n", path, SDL_GetError());
		}
		SDL_FreeSurface(loadedSurface);
	}
	return optimizedSurface;
}

void drawSurfaceTest(SDL_Surface *surface) {
	SDL_FillRect(surface,
		NULL,
		SDL_MapRGB(surface->format, 0x00, 0x00, 0x00));
	SDL_Rect rect1 = { .x = 10, .y = 10, .w = 100, .h = 100 };
	SDL_Rect rect2 = { .x = 110, .y = 10, .w = 100, .h = 100 };
	SDL_Rect rect3 = { .x = 210, .y = 10, .w = 100, .h = 100 };
	SDL_FillRect(surface, &rect1, 0xff115588);
	SDL_FillRect(surface, &rect2, 0xffee5533);
	SDL_FillRect(surface, &rect3, 0xffffbb44);
}

int main() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fputs(SDL_GetError(), stderr);
	}
	SDL_Window *window = SDL_CreateWindow("Game Engine",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640,
		480,
		SDL_WINDOW_SHOWN);
	if (window == NULL) {
		fputs(SDL_GetError(), stderr);
	}
	SDL_Surface *surface = SDL_GetWindowSurface(window);

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		fputs(IMG_GetError(), stderr);
	}

	SDL_Event e;
	char quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = 1; }
		}

		drawSurfaceTest(surface);
		SDL_UpdateWindowSurface(window);
	}

	if (surface != NULL) SDL_FreeSurface(surface);
	if (window != NULL) SDL_DestroyWindow(window);
	surface = NULL;
	window = NULL;
	SDL_Quit();
	return 0;
}
