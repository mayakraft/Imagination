#include <stdio.h>
#include "engine.h"
#include "draw.h"

int main(int argc, char* args[]) {
	InitParams params = InitParams {
		// .flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER,
		.flags = SDL_INIT_EVERYTHING,
		.title = "Game Engine",
		.width = 640,
		.height = 640,
	};

	std::string shaderDir = "./shaders/";

	GameEngine engine = initProgrammable(params);
	ShaderProgram program = createProgram(
		(shaderDir + "simple.vert").c_str(),
		(shaderDir + "kaleidoscope.frag").c_str());

	// event will be filled with some kind of input event (if exists)
	// https://wiki.libsdl.org/SDL2/SDL_Event
	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
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

		render(&program);
		SDL_GL_SwapWindow(engine.window);
	}

	return 0;
}
