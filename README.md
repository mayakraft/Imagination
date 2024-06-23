# Game Engine

cross-platform game engine written in C and modern OpenGL

# usage

1. build the library (see next section)
2. include this header `#include "include/engine.h"`
3. link to this library: `lib/libGameEngine.a` as well as `SDL` and `SDL_image`

Run this in the terminal to use Vulkan

```
export DYLD_LIBRARY_PATH=/Library/VulkanSDK/1.3.268.1/macOS/lib
```

# dependencies

- [SDL2](https://github.com/libsdl-org/SDL/releases)
- [SDL2 Image](https://github.com/libsdl-org/SDL_image/releases)

> some examples use [GLM](https://github.com/g-truc/glm/releases/), download and place `glm` in the examples folder `examples/glm/`.

# building this library

```
game-engine/
|
|-- bin/ (examples binaries)
|-- build/ (compiled object files)
|-- examples/ (examples source code)
|-- Frameworks/ (please create this directory)
|-- include/ (header files)
|-- lib/ (the compiled library)
|-- macos/ (Xcode project files for MacOS)
|-- src/ (the library's source code)
|-- win/ (Visual Studio project files for Windows)
```

## windows

Create a directory in the project root called "Frameworks".

Download the **development** releases for Visual Studio (VS) of:

- [SDL2](https://github.com/libsdl-org/SDL/releases)
- [SDL2 Image](https://github.com/libsdl-org/SDL_image/releases)

Place them in the "Frameworks" folder (and rename if necessary to match this naming). Your project should look like this:

```
game-engine/
|
|-- src/
|-- Frameworks/
|   |-- SDL2/
|   |-- SDL2_image/
|-- ...
```

Open the Visual Studio project and build and run.

## macos

Download the .dmg packages of:

- [SDL2](https://github.com/libsdl-org/SDL/releases)
- [SDL2 Image](https://github.com/libsdl-org/SDL_image/releases)

Place them in the "Frameworks" folder (and rename if necessary to match this naming). Your project should look like this:

```
game-engine/
|
|-- src/
|-- Frameworks/
|   |-- SDL2/
|   |-- SDL2_image/
|-- ...
```

run `make` and `make examples`, then run any of the examples, such as `./bin/maze3d`.

Alternatively, open the Xcode project and build and run.

run `make clean` to reset the project.

## linux

(I haven't tested on Linux yet)

The project will look for SDL2 inside the "Frameworks" folder. Download and build these two libraries:

- [SDL2](https://github.com/libsdl-org/SDL/releases)
- [SDL2 Image](https://github.com/libsdl-org/SDL_image/releases)

Place them in the "Frameworks" folder (and rename if necessary to match this naming). Your project should look like this:

```
game-engine/
|
|-- src/
|-- Frameworks/
|   |-- SDL2/
|   |-- SDL2_image/
|-- ...
```

run `make` and `make examples`, then run any of the examples, such as `./bin/maze3d`.

run `make clean` to reset the project.

# license

Apache
