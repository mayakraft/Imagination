// the classic screen saver "mystify your mind"
#include <cmath>
#include <stdio.h>
#include <vector>
#include "../src/engine.h"
#include "SDL2/SDL_render.h"

struct vec {
	int x;
	int y;
};

// float k (float n, float hue) { return (int)(n + hue / 30.0) % 12; }
// // hue: 0...360, saturation: 0...1, lightness: 0...1
// std::vector<float> hslToRgb (float h, float s, float l) {
// 	float a = s * fmin(l, 1 - l);
// 	float r = (l - a * fmax(-1, fmin(k(0, h) - 3, fmin(9 - k(0, h), 1)))) * 255.0;
// 	float g = (l - a * fmax(-1, fmin(k(8, h) - 3, fmin(9 - k(8, h), 1)))) * 255.0;
// 	float b = (l - a * fmax(-1, fmin(k(4, h) - 3, fmin(9 - k(4, h), 1)))) * 255.0;
// 	return std::vector<float> {{ r, g, b }};
// };


/*
 * Converts an HUE to r, g or b.
 * returns float in the set [0, 1].
 */
float hueToRgb(float p, float q, float t) {
	if (t < 0) t += 1;
	if (t > 1) t -= 1;
	if (t < 1./6) return p + (q - p) * 6 * t;
	if (t < 1./2) return q;
	if (t < 2./3) return p + (q - p) * (2./3 - t) * 6;
	return p;
}

////////////////////////////////////////////////////////////////////////

/*
 * Converts an HSL color value to RGB. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes h, s, and l are contained in the set [0, 1] and
 * returns RGB in the set [0, 255].
 */
std::vector<float> hslToRgb(float h, float s, float l) {
  if (0 == s) return std::vector<float> {{ l, l, l }};
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
		// .flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER,
		.flags = SDL_INIT_VIDEO,
		.title = "Game Engine",
		.width = SCREEN,
		.height = SCREEN,
	};

	GameEngine engine = initFixedFunction(params);
	// SDL_Texture* texture = loadTexture(&engine, "./src/8x8.png");

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

		SDL_SetRenderDrawBlendMode(engine.renderer, SDL_BLENDMODE_ADD);
		SDL_SetRenderDrawColor(engine.renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(engine.renderer);
		SDL_SetRenderDrawColor(engine.renderer, 0xFF, 0xFF, 0xFF, 0xFF);

		for (int i = 0; i < NUM_POLYS; i += 1) {
			float hue = fmod(frame / 200. + (float)i / (float)NUM_POLYS, 1.0);
			float saturation = 1.0;
			// float lightness = (sin(frame / 400. + i) + 1) * 0.5 * 0.3 + 0.2;
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
