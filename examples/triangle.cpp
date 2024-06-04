#include <stdio.h>
#include "../src/engine.h"
#include "../src/draw.h"

int main(int argc, char* args[]) {
	InitParams params = InitParams {
		// .flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER,
		.flags = SDL_INIT_EVERYTHING,
		.title = "Game Engine",
		.width = 640,
		.height = 640,
	};

	GameEngine engine = initFixedFunction(params);
	SDL_Texture* texture = loadTexture(&engine, "./src/8x8.png");

	// event will be filled with some kind of input event (if exists)
	// https://wiki.libsdl.org/SDL2/SDL_Event
	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}

		// Basic draw test
		SDL_SetRenderDrawColor(engine.renderer, 0x30, 0x30, 0x30, 0xFF);
		SDL_RenderClear(engine.renderer);
		drawTriangle(&engine, texture);
		SDL_RenderPresent(engine.renderer);
	}

	return 0;
}
