//
//  main.cpp
//  Imagine
//
//  Created by Maya Kraft on 6/1/24.
//

#include <stdio.h>
#include "Imagine.h"

void draw(ImagineEngine *engine) {
    SDL_FillRect(engine->surface,
                 NULL,
                 SDL_MapRGB(engine->surface->format, 0x00, 0x00, 0x00));
    SDL_Rect rect1 = SDL_Rect { .x = 10, .y = 10, .w = 100, .h = 100 };
    SDL_Rect rect2 = SDL_Rect { .x = 110, .y = 10, .w = 100, .h = 100 };
    SDL_Rect rect3 = SDL_Rect { .x = 210, .y = 10, .w = 100, .h = 100 };
    // unsigned int color = 0xffffffff;
    SDL_FillRect(engine->surface, &rect1, 0xff115588);
    SDL_FillRect(engine->surface, &rect2, 0xffee5533);
    SDL_FillRect(engine->surface, &rect3, 0xffffbb44);
}

int main(int argc, char* args[]) {
    ImagineInitParams params = ImagineInitParams {
        .title = "Imagination Engine",
        .width = 640,
        .height = 480,
    };
    ImagineEngine engine = init(params);

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
                //Select surfaces based on key press
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
        draw(&engine);
        SDL_UpdateWindowSurface(engine.window);
    }

    return 0;
}
