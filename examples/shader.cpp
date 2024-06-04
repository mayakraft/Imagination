#include <stdio.h>
#include "../src/engine.h"

int main(int argc, char* args[]) {
	InitParams params = InitParams {
		.flags = SDL_INIT_VIDEO,
		.title = "Game Engine",
		.width = 640,
		.height = 640,
	};

	GameEngine engine = initProgrammable(params);
	ShaderProgram program = createProgram(
		"./src/shaders/simple.vert",
		"./src/shaders/kaleidoscope.frag");

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}

		render(&program);
		SDL_GL_SwapWindow(engine.window);
	}

	return 0;
}
