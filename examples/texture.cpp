#include <stdio.h>
#include "../src/engine.h"

int main(int argc, char* args[]) {
	InitParams params = InitParams {
		.flags = SDL_INIT_EVERYTHING,
		.title = "Game Engine",
		.width = 640,
		.height = 480,
	};
	GameEngine engine = init(params);

	SDL_Texture* texture = loadTexture(&engine, "examples/images/copper.png");

	// event will be filled with some kind of input event (if exists)
	// https://wiki.libsdl.org/SDL2/SDL_Event
	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
				dealloc(&engine);
				return 0;
			} else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_UP:
						printf("up\n");
					break;
					case SDLK_DOWN:
						printf("down\n");
					break;
					case SDLK_LEFT:
						printf("left\n");
					break;
					case SDLK_RIGHT:
						printf("right\n");
					break;
					default:
						printf("unbound\n");
					break;
				}
			}
		}
		// Clear screen
		SDL_SetRenderDrawColor(engine.renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(engine.renderer);
		// Render texture to screen
		SDL_RenderCopy(engine.renderer, texture, NULL, NULL);
		// Update screen
		SDL_RenderPresent(engine.renderer);
	}

	return 0;
}
