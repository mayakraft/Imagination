#include <stdio.h>
#include "../include/engine.h"

int main() {
	InitParams params = {
		.flags = SDL_INIT_EVERYTHING,
		.title = "Game Engine",
		.width = 640,
		.height = 480,
	};

	GameEngine engine = initSDLEngine(params);
	SDL_Texture* texture = loadSDLTexture(engine.renderer, "examples/images/copper.png");

	SDL_Event e;
	char quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = 1;
				dealloc(&engine);
				return 0;
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

	dealloc(&engine);
	return 0;
}
