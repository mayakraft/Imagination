//
//  Imagination.h
//  Imagination
//
//  Created by Maya Kraft on 5/31/24.
//

#include <stdio.h>
#include <SDL2/SDL.h>

struct ImaginationInitParams {
    int width;
    int height;
    const char* title;
};

class Imagination {
    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
    // SDL_Surface* image = NULL;
    // turn into arrays
    // SDL_Joystick* gameController = NULL;

public:
    Imagination(ImaginationInitParams params);
    ~Imagination();
    
    void drawLoop();
    
    void drawTest();
    
    void joystickMainLoop();
};
