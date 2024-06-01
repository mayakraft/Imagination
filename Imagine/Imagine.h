//
//  Imagine.h
//  Imagine
//
//  Created by Maya Kraft on 6/1/24.
//

#ifndef Imagine_h
#define Imagine_h

#include <vector>
#include <stdio.h>
#include <SDL2/SDL.h>

struct ImagineInitParams {
    int width;
    int height;
    const char* title;
};

struct ImagineEngine {
    SDL_Window* window;
    SDL_Surface* surface;
    std::vector<SDL_Joystick*> controllers;
};

ImagineEngine init(ImagineInitParams params) {
    //if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        throw std::runtime_error(SDL_GetError());
    }
        
    SDL_Window *window = SDL_CreateWindow(
                                   params.title,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   params.width,
                                   params.height,
                                   SDL_WINDOW_SHOWN);
    
    if(window == NULL) {
        throw std::runtime_error(SDL_GetError());
    }
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    ImagineEngine engine = ImagineEngine {
        .window = window,
        .surface = surface,
        .controllers = std::vector<SDL_Joystick*>(),
    };
    return engine;
}

void dealloc(ImagineEngine* engine) {
    SDL_FreeSurface(engine->surface);
    SDL_DestroyWindow(engine->window);
    engine->surface = NULL;
    engine->window = NULL;
    SDL_Quit();
}

void drawLoop(ImagineEngine* engine) {
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
        // update, draw
        SDL_UpdateWindowSurface(engine->window);
    }
}

void drawTest(ImagineEngine *engine) {
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

    //Update the surface
    SDL_UpdateWindowSurface(engine->window);
}

#endif /* Imagine_h */
