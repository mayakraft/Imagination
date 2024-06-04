#include <stdio.h>
#ifdef _WIN32
#include "SDL_render.h"
#else
#include "SDL2/SDL_render.h"
#endif
#include "engine.cpp"
//#include <CoreFoundation/CoreFoundation.h>

void macBundlePath() {
//    CFBundleRef mainBundle = CFBundleGetMainBundle();
//    CFURLRef resourcesURL = CFBundleCopyBundleURL(mainBundle);
//    CFStringRef str = CFURLCopyFileSystemPath( resourcesURL, kCFURLPOSIXPathStyle );
//    CFRelease(resourcesURL);
//    char path[PATH_MAX];
//    CFStringGetCString( str, path, FILENAME_MAX, kCFStringEncodingASCII );
//    CFRelease(str);
//    printf(path);
}

void drawQuad(GameEngine *engine, SDL_Texture *texture) {
	SDL_Color col{ 0xff, 0xff, 0xff, 0xff };
		std::vector<SDL_Vertex> verticies{
			{{ 0.f, 0.f },	col,	{ 0.0f, 0.0f }},
			{{ 1.f, 0.f },	col,	{ 1.0f, 0.0f }},
			{{ 1.f, 1.f },	col,	{ 0.0f, 1.0f }},
			{{ 0.f, 1.f },	col,	{ 1.0f, 1.0f }},
		};
		std::vector<int> indexList = { 0, 1, 2, 2, 3, 1 };
		SDL_RenderGeometry(engine->renderer, texture, verticies.data(), (int)verticies.size(), indexList.data(), (int)indexList.size());
}

void drawTriangle(GameEngine *engine, SDL_Texture *texture) {
	SDL_Color col{ 0xff, 0xff, 0xff, 0xff };
		std::vector<SDL_Vertex> verticies{
			{{ 50.f, 450.f },	col,	{ 0.0f, 0.0f }},
			{{ 550.f, 450.f },	col,	{ 1.0f, 0.0f }},
			{{ 300.f, 50.f },	col,	{ 0.0f, 1.0f }},
		};
		std::vector<int> indexList = { 0, 1, 2 };
		SDL_RenderGeometry(engine->renderer, texture, verticies.data(), (int)verticies.size(), indexList.data(), (int)indexList.size());
}

void drawRendererTest(GameEngine* engine) {
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
	InitParams params = InitParams {
		// .flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER,
		.flags = SDL_INIT_EVERYTHING,
		.title = "Game Engine",
		.width = 640,
		.height = 640,
	};

	// std::string shaderDir = "../shaders/";
	std::string shaderDir = "./shaders/";

	GameEngine engine = initProgrammable(params);
	ShaderProgram program = createProgram(
		(shaderDir + "simple.vert").c_str(),
		(shaderDir + "kaleidoscope.frag").c_str());

	// SDL_Texture* texture = loadTexture(&engine, "./src/8x8.png");

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
		// update, draw
		// SDL_UpdateWindowSurface(engine.window);

		// // Basic draw test
		// SDL_SetRenderDrawColor(engine.renderer, 0x30, 0x30, 0x30, 0xFF);
		// SDL_RenderClear(engine.renderer);
		// drawTriangle(&engine, texture);
		// SDL_RenderPresent(engine.renderer);
		render(&program);
		//Update screen
		SDL_GL_SwapWindow(engine.window);

		// drawRendererTest(&engine);

		// Render texture to screen
		// SDL_RenderCopy(engine.renderer, texture, NULL, NULL);
		// Update screen
		// SDL_RenderPresent(engine.renderer);
	}

	return 0;
}
