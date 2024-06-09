// this is the classic screen saver "mystify your mind"
#include <cmath>
#include <stdio.h>
#include <vector>
#include "../src/engine.h"

struct vec {
	int x;
	int y;
};

float hueToRgb(float p, float q, float t) {
	if (t < 0) t += 1;
	if (t > 1) t -= 1;
	if (t < 1./6) return p + (q - p) * 6 * t;
	if (t < 1./2) return q;
	if (t < 2./3) return p + (q - p) * (2./3 - t) * 6;
	return p;
}

// h, s, l between 0...1. r, g, b between 0...255
std::vector<float> hslToRgb(float h, float s, float l) {
  if (0 == s) return std::vector<float> {{ l * 255, l * 255, l * 255 }};
	float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
	float p = 2 * l - q;
	float r = hueToRgb(p, q, h + 1./3) * 255;
	float g = hueToRgb(p, q, h) * 255;
	float b = hueToRgb(p, q, h - 1./3) * 255;
	return std::vector<float> {{ r, g, b }};
}

int main(int argc, char* args[]) {
	int SCREEN = 640;
	int NUM_POLYS = 10;
	int POLY_SIZE = 4;
	int frame = 0;

	InitParams params = InitParams {
		.flags = SDL_INIT_VIDEO,
		.title = "Mystify Your Mind",
		.width = SCREEN,
		.height = SCREEN,
	};
	GameEngine engine = init2D(params);

	std::vector<vec> points = std::vector<vec>();
	std::vector<vec> velocities = std::vector<vec>();
	for (int i = 0; i < 10 * NUM_POLYS; i += 1) {
		points.push_back(vec {
			.x = rand() % SCREEN,
			.y = rand() % SCREEN
		});
		velocities.push_back(vec {
			.x = (rand() % 2) * 2 - 1,
			.y = (rand() % 2) * 2 - 1
		});
	}

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = true; }
		}

		frame += 1;

		for (int i = 0; i < NUM_POLYS * POLY_SIZE; i += 1) {
			points[i].x += velocities[i].x;
			points[i].y += velocities[i].y;
			if (points[i].x < 0) {
				points[i].x = 0;
				velocities[i].x = +abs(velocities[i].x);
			}
			if (points[i].y < 0) {
				points[i].y = 0;
				velocities[i].y = +abs(velocities[i].y);
			}
			if (points[i].x > SCREEN) {
				points[i].x = SCREEN;
				velocities[i].x = -abs(velocities[i].x);
			}
			if (points[i].y > SCREEN) {
				points[i].y = SCREEN;
				velocities[i].y = -abs(velocities[i].y);
			}
		}

		SDL_SetRenderDrawColor(engine.renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(engine.renderer);
		SDL_SetRenderDrawColor(engine.renderer, 0xFF, 0xFF, 0xFF, 0xFF);

		for (int i = 0; i < NUM_POLYS; i += 1) {
			float hue = fmod(frame / 200. + (float)i / (float)NUM_POLYS, 1.0);
			float saturation = 1.0;
			float lightness = 0.5;
			std::vector<float> rgb = hslToRgb(hue, saturation, lightness);
			SDL_SetRenderDrawColor(engine.renderer, rgb[0], rgb[1], rgb[2], 0xFF);
			for (int j = 0; j < POLY_SIZE; j += 1) {
				SDL_RenderDrawLine(
					engine.renderer,
					points[i * POLY_SIZE + j].x,
					points[i * POLY_SIZE + j].y,
					points[i * POLY_SIZE + (j + 1) % POLY_SIZE].x,
					points[i * POLY_SIZE + (j + 1) % POLY_SIZE].y
				);
			}
		}

		SDL_RenderPresent(engine.renderer);
	}

	return 0;
}
