//
//  main.cpp
//  Imagination
//
//  Created by Maya Kraft on 5/31/24.
//

#include <stdio.h>
#include "Imagination.h"

int main(int argc, char* args[])
{
    auto params = ImaginationInitParams {
        .title = "",
        .width = 640,
        .height = 480,
    };
    Imagination engine = Imagination(params);
    engine.drawTest();
    engine.drawLoop();
    return 0;
}
