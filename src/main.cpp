#include <stdio.h>
#include "imagine.h"

void drawRendererTest(ImagineEngine* engine) {
	SDL_SetRenderDrawColor(engine->renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(engine->renderer);
	// SDL_RenderFillRect(engine->renderer, NULL);
	SDL_Rect rect1 = SDL_Rect { .x = 10, .y = 10, .w = 100, .h = 100 };
	SDL_Rect rect2 = SDL_Rect { .x = 110, .y = 10, .w = 100, .h = 100 };
	SDL_Rect rect3 = SDL_Rect { .x = 210, .y = 10, .w = 100, .h = 100 };
	SDL_SetRenderDrawColor(engine->renderer, 0x11, 0x55, 0x88, 0xFF);
	SDL_RenderFillRect(engine->renderer, &rect1);
	SDL_SetRenderDrawColor(engine->renderer, 0xee, 0x55, 0x33, 0xFF);
	SDL_RenderFillRect(engine->renderer, &rect2);
	SDL_SetRenderDrawColor(engine->renderer, 0xff, 0xbb, 0x44, 0xFF);
	SDL_RenderFillRect(engine->renderer, &rect3);
}

int main(int argc, char* args[]) {
	ImagineInitParams params = ImagineInitParams {
		.title = "Imagine Engine",
		.width = 640,
		.height = 480,
	};
	ImagineEngine engine = init(params);

	// SDL_Texture* texture = loadTexture(&engine, "images/copper.png");

	// event will be filled with some kind of input event (if exists)
	// https://wiki.libsdl.org/SDL2/SDL_Event
	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				printf("USER QUIT REQUEST\n");
				quit = true;
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
		// update, draw
		// SDL_UpdateWindowSurface(engine.window);

		// Clear screen
		SDL_SetRenderDrawColor(engine.renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(engine.renderer);

		drawRendererTest(&engine);
		SDL_RenderPresent(engine.renderer);

		// Render texture to screen
		// SDL_RenderCopy(engine.renderer, texture, NULL, NULL);
		// Update screen
		// SDL_RenderPresent(engine.renderer);
	}

	return 0;
}
