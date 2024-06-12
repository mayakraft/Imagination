#include <stdio.h>
#include "engine.h"
// #include "draw.h"
#include "shader.h"

int main(int argc, char* args[]) {
	int SCREEN = 640;
	int frame = 0;

	InitParams params;
	params.flags = SDL_INIT_VIDEO;
	params.title = "SDL Triangle";
	params.width = SCREEN;
	params.height = SCREEN;
	GameEngine engine = init2D(params);

	SDL_Event e;
	char quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = 1; }
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
		SDL_RenderGeometry(engine.renderer, NULL, vertices, 3, indices, 3);
		SDL_RenderPresent(engine.renderer);

		frame += 1;
	}

	dealloc(&engine);
	return 0;
}
