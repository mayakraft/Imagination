// this is the classic screen saver "mystify your mind"
#include <math.h>
#include <stdio.h>
#include "./misc/colors.c"
#include "../src/engine.h"

typedef struct coord {
	int x;
	int y;
} coord;

typedef struct particle {
	coord position;
	coord velocity;
} particle;

void particleUpdate(particle *p, GameEngine *engine) {
	p->position.x += p->velocity.x;
	p->position.y += p->velocity.y;
	if (p->position.x < 0) {
		p->position.x = 0;
		p->velocity.x = +abs(p->velocity.x);
	}
	if (p->position.y < 0) {
		p->position.y = 0;
		p->velocity.y = +abs(p->velocity.y);
	}
	if (p->position.x > engine->width) {
		p->position.x = engine->width;
		p->velocity.x = -abs(p->velocity.x);
	}
	if (p->position.y > engine->height) {
		p->position.y = engine->height;
		p->velocity.y = -abs(p->velocity.y);
	}
}

int main(int argc, char* args[]) {
	int NUM_POLYS = 10;
	int POLY_SIZE = 4;
	int NUM_DOTS = NUM_POLYS * POLY_SIZE;
	int frame = 0;

	InitParams params;
	params.flags = SDL_INIT_VIDEO;
	params.title = "Mystify Your Mind";
	params.width = 640;
	params.height = 640;
	GameEngine engine = init2D(params);

	particle pts[NUM_DOTS];
	for (int i = 0; i < NUM_DOTS; i += 1) {
		pts[i].position.x = rand() % engine.width;
		pts[i].position.y = rand() % engine.height;
		pts[i].velocity.x = (rand() % 2) * 2 - 1;
		pts[i].velocity.y = (rand() % 2) * 2 - 1;
	}

	SDL_Event e;
	char quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = 1; }
			else if (e.type == SDL_KEYDOWN) {
				// printf( "%c (0x%04X)\n", (char)e.key.keysym.sym, e.key.keysym.sym );
				switch (e.key.keysym.sym) {
					case SDLK_f:
					// float aspect;
					SDL_SetWindowFullscreen(engine.window, SDL_WINDOW_FULLSCREEN);
					int w, h;
					// SDL_GetWindowSize(engine.window, &w, &h);
					SDL_GetRendererOutputSize(engine.renderer, &w, &h);
					engine.width = w;
					engine.height = h;
					for (int i = 0; i < NUM_DOTS; i += 1) {
						pts[i].position.x = rand() % engine.width;
						pts[i].position.y = rand() % engine.height;
					}
					break;
				}
			}
		}

		frame += 1;

		for (int i = 0; i < NUM_DOTS; i += 1) {
			particleUpdate(&pts[i], &engine);
			// pts[i].update(&engine);
		}

		SDL_SetRenderDrawColor(engine.renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(engine.renderer);
		SDL_SetRenderDrawColor(engine.renderer, 0xFF, 0xFF, 0xFF, 0xFF);

		for (int i = 0; i < NUM_POLYS; i += 1) {
			float hue = fmod(frame / 200. + (float)i / (float)NUM_POLYS, 1.0);
			float saturation = 1.0;
			float lightness = 0.5;
			unsigned char red, green, blue;
			hslToRgb(hue, saturation, lightness, &red, &green, &blue);
			SDL_SetRenderDrawColor(engine.renderer, red, green, blue, 0xFF);
			for (int j = 0; j < POLY_SIZE; j += 1) {
				SDL_RenderDrawLine(
					engine.renderer,
					pts[i * POLY_SIZE + j].position.x,
					pts[i * POLY_SIZE + j].position.y,
					pts[i * POLY_SIZE + (j + 1) % POLY_SIZE].position.x,
					pts[i * POLY_SIZE + (j + 1) % POLY_SIZE].position.y
				);
			}
		}

		SDL_RenderPresent(engine.renderer);
	}

	return 0;
}
