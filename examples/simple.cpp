#include <stdio.h>
#include "../src/imagine.h"

void drawSurfaceTest(SDL_Surface *surface) {
	SDL_FillRect(surface,
		NULL,
		SDL_MapRGB(surface->format, 0x00, 0x00, 0x00));
	SDL_Rect rect1 = SDL_Rect { .x = 10, .y = 10, .w = 100, .h = 100 };
	SDL_Rect rect2 = SDL_Rect { .x = 110, .y = 10, .w = 100, .h = 100 };
	SDL_Rect rect3 = SDL_Rect { .x = 210, .y = 10, .w = 100, .h = 100 };
	SDL_FillRect(surface, &rect1, 0xff115588);
	SDL_FillRect(surface, &rect2, 0xffee5533);
	SDL_FillRect(surface, &rect3, 0xffffbb44);
}

int main(int argc, char* args[]) {
	// if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		throw std::runtime_error(SDL_GetError());
	}
	SDL_Window *window = SDL_CreateWindow("Imagine Engine",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640,
		480,
		SDL_WINDOW_SHOWN);
	if (window == NULL) {
		throw std::runtime_error(SDL_GetError());
	}
	SDL_Surface *surface = SDL_GetWindowSurface(window);

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		throw std::runtime_error(IMG_GetError());
	}

	// event will be filled with some kind of input event (if exists)
	// https://wiki.libsdl.org/SDL2/SDL_Event
	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				printf("USER QUIT REQUEST\n");
				quit = true;
			}
		}

		// update, draw
		drawSurfaceTest(surface);
		SDL_UpdateWindowSurface(window);
	}

	return 0;
}
