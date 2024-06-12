#include <stdio.h>
#include "../src/engine.h"
#include "../src/textures.h"

int main(int argc, char* args[]) {
	InitParams params = InitParams {
		.flags = SDL_INIT_EVERYTHING,
		.title = "Game Engine",
		.width = 640,
		.height = 480,
	};

	GameEngine engine = init2D(params);
	SDL_Texture* texture = loadSDLTexture(engine.renderer, "examples/images/copper.png");

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
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
