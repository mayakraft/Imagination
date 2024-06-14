# Game Engine

cross-platform C/C++ game engine in modern OpenGL

# try it

```make
make && make examples
make runmaze
```

# developers

Dependencies

- [SDL2](https://github.com/libsdl-org/SDL/releases)
- [SDL2 Image](https://github.com/libsdl-org/SDL_image/releases)
- [GLEW](https://github.com/nigels-com/glew/releases)

## windows

Create a directory in the project root called "external".

Download the **development** releases for Visual Studio (VS) of:

- [SDL2](https://github.com/libsdl-org/SDL/releases)
- [SDL2 Image](https://github.com/libsdl-org/SDL_image/releases)

Also the Windows release of GLEW:

- [GLEW](https://glew.sourceforge.net/)

Place them in the "external" folder (and rename if necessary). Your project should look like this:

```
game-engine/
|
|-- src/
|-- macos/
|-- win/
|-- external/
    |-- GLEW/
    |-- SDL2/
    |-- SDL2_image/
```

Open the Visual Studio project and build and run.

## macos

Install GLEW into your system, download SDL and place locally in the project folder.

Install GLEW using `brew` or [download GLEW](https://glew.sourceforge.net/) and follow the instructions to install via `make`. This places `libGLEW.2.2.0.dylib` into `usr/local/lib` and this project is expecting to find it here (both the makefile and Xcode project).

Download these SDL frameworks for MacOS, place them in the project root.

- [SDL2](https://github.com/libsdl-org/SDL/releases)
- [SDL2 Image](https://github.com/libsdl-org/SDL_image/releases)

Your project should look like this:

```
game-engine/
|
|-- src/
|-- macos/
|-- win/
|-- SDL2.framework
|-- SDL2_image.framework
```

run `make` and `make run`, or open the Xcode project and build and run.

## linux

# license

Apache
