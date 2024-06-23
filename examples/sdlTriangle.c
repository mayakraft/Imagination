// this demonstrates rendering a triangle
// using the SDL hardware-accelerated renderer.
// this approach is 2D only.
#include <math.h>
#include "../include/engine.h"

int main() {
	int SCREEN = 640;
	int frame = 0;
	char showTexture = 0;

	InitParams params = {
		.flags = SDL_INIT_VIDEO,
		.title = "SDL Triangle",
		.width = SCREEN,
		.height = SCREEN,
	};

	GameEngine engine = initSDLEngine(params);
	SDL_Texture* texture = loadSDLTexture(engine.renderer, "examples/images/wall.png");

	SDL_Event e;
	char quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = 1; }
			else if (e.type == SDL_KEYDOWN) {
				showTexture = !showTexture;
			}
		}

		float r = (engine.width / 3.0);
		float a = frame / 100.0f;
		const float xy[] = {
			(engine.width / 2.0f + cosf(a) * r),
			(engine.height / 2.0f + sinf(a) * r),
			(engine.width / 2.0f + cosf(a + 2 * M_PI / 3) * r),
			(engine.height / 2.0f + sinf(a + 2 * M_PI / 3) * r),
			(engine.width / 2.0f + cosf(a + 2 * M_PI / 3 * 2) * r),
			(engine.height / 2.0f + sinf(a + 2 * M_PI / 3 * 2) * r),
		};

		SDL_Vertex vertices[] = {
			{{ xy[0], xy[1] }, { 0xFF, 0x00, 0x00, 0xFF }, { 0.5f, 1.0f }},
			{{ xy[2], xy[3] }, { 0x00, 0xFF, 0x00, 0xFF }, { 0.0f, 0.0f }},
			{{ xy[4], xy[5] }, { 0x00, 0x00, 0xFF, 0xFF }, { 1.0f, 0.0f }},
		};
		int indices[] = { 0, 1, 2 };

		SDL_SetRenderDrawColor(engine.renderer, 0x29, 0x2D, 0x2B, 0xFF);
		SDL_RenderClear(engine.renderer);
		if (showTexture) {
			SDL_RenderGeometry(engine.renderer, texture, vertices, 3, indices, 3);
		} else {
			SDL_RenderGeometry(engine.renderer, NULL, vertices, 3, indices, 3);
		}
		SDL_RenderPresent(engine.renderer);

		frame += 1;
	}

	dealloc(&engine);
	return 0;
}
