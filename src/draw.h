#include "SDL2/SDL_render.h"
#include "engine.h"

void drawQuad(GameEngine *engine, SDL_Texture *texture) {
	SDL_Color col{ 0xff, 0xff, 0xff, 0xff };
		std::vector<SDL_Vertex> verticies{
			{{ 0.f, 0.f },	col,	{ 0.0f, 0.0f }},
			{{ 1.f, 0.f },	col,	{ 1.0f, 0.0f }},
			{{ 1.f, 1.f },	col,	{ 0.0f, 1.0f }},
			{{ 0.f, 1.f },	col,	{ 1.0f, 1.0f }},
		};
		std::array<int, 6> indexList = { 0, 1, 2, 2, 3, 1 };
		SDL_RenderGeometry(engine->renderer, texture, verticies.data(), (int)verticies.size(), indexList.data(), (int)indexList.size());
}

void drawTriangle(GameEngine *engine, SDL_Texture *texture) {
	SDL_Color col{ 0xff, 0xff, 0xff, 0xff };
		std::vector<SDL_Vertex> verticies{
			{{ 50.f, 450.f },	col,	{ 0.0f, 0.0f }},
			{{ 550.f, 450.f },	col,	{ 1.0f, 0.0f }},
			{{ 300.f, 50.f },	col,	{ 0.0f, 1.0f }},
		};
		std::array<int, 3> indexList = { 0, 1, 2 };
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
