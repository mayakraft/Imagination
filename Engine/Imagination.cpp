//
//  Imagination.cpp
//  Imagination
//
//  Created by Maya Kraft on 5/31/24.
//

#include "Imagination.h"
#include <stdexcept>

const int JOYSTICK_DEAD_ZONE = 8000;

Imagination::Imagination(ImaginationInitParams params) {
    //if (SDL_Init(SDL_INIT_VIDEO) < 0)
    //if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    //if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        throw std::runtime_error(SDL_GetError());
    }
    
    window = SDL_CreateWindow(params.title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, params.width, params.height, SDL_WINDOW_SHOWN);

    if(window == NULL) {
        throw std::runtime_error(SDL_GetError());
    }
    screenSurface = SDL_GetWindowSurface(window);
}

Imagination::~Imagination() {
    SDL_FreeSurface(screenSurface);
    SDL_DestroyWindow(window);
    screenSurface = NULL;
    window = NULL;
    SDL_Quit();
}

void Imagination::drawLoop() {
    SDL_Event e;
    bool quit = false;
    while (quit == false) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
    }
}

void Imagination::drawTest() {
    SDL_FillRect(screenSurface,
                 NULL,
                 SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
    
    SDL_Rect rect1 = SDL_Rect { .x = 10, .y = 10, .w = 100, .h = 100 };
    SDL_Rect rect2 = SDL_Rect { .x = 110, .y = 10, .w = 100, .h = 100 };
    SDL_Rect rect3 = SDL_Rect { .x = 210, .y = 10, .w = 100, .h = 100 };
//    unsigned int color = 0xffffffff;
    SDL_FillRect(screenSurface, &rect1, 0xff115588);
    SDL_FillRect(screenSurface, &rect2, 0xffee5533);
    SDL_FillRect(screenSurface, &rect3, 0xffffbb44);

    //Update the surface
    SDL_UpdateWindowSurface(window);
}

void Imagination::joystickMainLoop() {
    
    //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

    //Normalized direction
    int xDir = 0;
    int yDir = 0;

    //While application is running
    while( !quit )
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
            else if( e.type == SDL_JOYAXISMOTION )
            {
                //Motion on controller 0
                if( e.jaxis.which == 0 )
                {
                    //X axis motion
                    if( e.jaxis.axis == 0 )
                    {
                        //Left of dead zone
                        if( e.jaxis.value < -JOYSTICK_DEAD_ZONE )
                        {
                            xDir = -1;
                        }
                        //Right of dead zone
                        else if( e.jaxis.value > JOYSTICK_DEAD_ZONE )
                        {
                            xDir =  1;
                        }
                        else
                        {
                            xDir = 0;
                        }
                    }
                    //Y axis motion
                    else if( e.jaxis.axis == 1 )
                    {
                        //Below of dead zone
                        if( e.jaxis.value < -JOYSTICK_DEAD_ZONE )
                        {
                            yDir = -1;
                        }
                        //Above of dead zone
                        else if( e.jaxis.value > JOYSTICK_DEAD_ZONE )
                        {
                            yDir =  1;
                        }
                        else
                        {
                            yDir = 0;
                        }
                    }
                }
            }
        }

        //Clear screen
//        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
//        SDL_RenderClear(renderer);

        //Calculate angle
        double joystickAngle = atan2( (double)yDir, (double)xDir ) * ( 180.0 / M_PI );
                
        //Correct angle
        if( xDir == 0 && yDir == 0 )
        {
            joystickAngle = 0;
        }

        //Render joystick 8 way angle
//        gArrowTexture.render( ( SCREEN_WIDTH - gArrowTexture.getWidth() ) / 2, ( SCREEN_HEIGHT - gArrowTexture.getHeight() ) / 2, NULL, joystickAngle );

        //Update screen
//        SDL_RenderPresent(renderer);
    }
    
}
