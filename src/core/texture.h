#pragma once

#include "glad.h"
#ifdef _WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

GLuint loadGLTexture(unsigned char *data, int width, int height);
GLuint loadGLTextureFromFile(const char* path);

GLuint loadGLAlphaTexture(unsigned char *data, int width, int height);
GLuint loadGLAlphaTextureFromFile(const char* path);

GLuint updateGLTexture(GLuint texture, unsigned char *data, int width, int height);

SDL_Texture* loadSDLTexture(SDL_Renderer *renderer, const char* path);

#ifdef __cplusplus
}
#endif
