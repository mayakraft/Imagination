//
//  Imagine.h
//  Imagine
//
//  Created by Maya Kraft on 6/1/24.
//

#ifndef Imagine_h
#define Imagine_h

#include <vector>
#include <string>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

struct ImagineInitParams {
    int width;
    int height;
    const char* title;
};

struct ImagineEngine {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* surface;
    std::vector<SDL_Joystick*> controllers;
};

ImagineEngine initWithSurface(ImagineInitParams params) {
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
    if (window == NULL) {
        throw std::runtime_error(SDL_GetError());
    }
    SDL_Surface *surface = SDL_GetWindowSurface(window);

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        throw std::runtime_error(IMG_GetError());
    }

    ImagineEngine engine = ImagineEngine {
        .window = window,
        .surface = surface,
        .controllers = std::vector<SDL_Joystick*>(),
    };
    return engine;
}

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
    if (window == NULL) {
        throw std::runtime_error(SDL_GetError());
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        throw std::runtime_error(SDL_GetError());
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        throw std::runtime_error(IMG_GetError());
    }

    ImagineEngine engine = ImagineEngine {
        .window = window,
        .renderer = renderer,
        .controllers = std::vector<SDL_Joystick*>(),
    };

    //Initialize renderer color
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    return engine;
}

SDL_Surface* loadSurface(ImagineEngine* engine, std::string path) {
    //The final optimized image
    SDL_Surface* optimizedSurface = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    } else {
        optimizedSurface = SDL_ConvertSurface(loadedSurface, engine->surface->format, 0);
        if (optimizedSurface == NULL) {
            printf( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }
        SDL_FreeSurface(loadedSurface);
    }
    return optimizedSurface;
}

SDL_Texture* loadTexture(ImagineEngine* engine, std::string path) {
    // The final texture
    SDL_Texture* newTexture = NULL;
    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    } else {
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(engine->renderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }
        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}

void dealloc(ImagineEngine* engine) {
    if (engine->surface != NULL) SDL_FreeSurface(engine->surface);
    if (engine->renderer != NULL) SDL_DestroyRenderer(engine->renderer);
    if (engine->window != NULL) SDL_DestroyWindow(engine->window);
    engine->surface = NULL;
    engine->renderer = NULL;
    engine->window = NULL;
    IMG_Quit();
    SDL_Quit();
}

//void drawLoop(ImagineEngine* engine) {
//    // event will be filled with some kind of input event (if exists)
//    // https://wiki.libsdl.org/SDL2/SDL_Event
//    SDL_Event e;
//    bool quit = false;
//    while (!quit) {
//        while (SDL_PollEvent(&e)) {
//            if (e.type == SDL_QUIT) {
//                quit = true;
//            }
//        }
//        // update, draw
//        SDL_UpdateWindowSurface(engine->window);
//    }
//}

void drawSurfaceTest(ImagineEngine *engine) {
    SDL_FillRect(engine->surface,
                 NULL,
                 SDL_MapRGB(engine->surface->format, 0x00, 0x00, 0x00));
    SDL_Rect rect1 = SDL_Rect { .x = 10, .y = 10, .w = 100, .h = 100 };
    SDL_Rect rect2 = SDL_Rect { .x = 110, .y = 10, .w = 100, .h = 100 };
    SDL_Rect rect3 = SDL_Rect { .x = 210, .y = 10, .w = 100, .h = 100 };
    SDL_FillRect(engine->surface, &rect1, 0xff115588);
    SDL_FillRect(engine->surface, &rect2, 0xffee5533);
    SDL_FillRect(engine->surface, &rect3, 0xffffbb44);
}

void drawRendererTest(ImagineEngine* engine) {
    SDL_SetRenderDrawColor(engine->renderer, 0x00, 0x00, 0x00, 255);
    SDL_RenderClear(engine->renderer);
//    SDL_RenderFillRect(engine->renderer, NULL);
    SDL_Rect rect1 = SDL_Rect { .x = 10, .y = 10, .w = 100, .h = 100 };
    SDL_Rect rect2 = SDL_Rect { .x = 110, .y = 10, .w = 100, .h = 100 };
    SDL_Rect rect3 = SDL_Rect { .x = 210, .y = 10, .w = 100, .h = 100 };
    SDL_SetRenderDrawColor(engine->renderer, 0x11, 0x55, 0x88, 255);
    SDL_RenderFillRect(engine->renderer, &rect1);
    SDL_SetRenderDrawColor(engine->renderer, 0xee, 0x55, 0x33, 255);
    SDL_RenderFillRect(engine->renderer, &rect2);
    SDL_SetRenderDrawColor(engine->renderer, 0xff, 0xbb, 0x44, 255);
    SDL_RenderFillRect(engine->renderer, &rect3);
}

#endif /* Imagine_h */
