#pragma once

#include <vulkan/vulkan.h>
#ifdef _WIN32
#include <SDL.h>
#include <SDL_vulkan.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void initVulkan(SDL_Window *window, const char* windowTitle);

#ifdef __cplusplus
}
#endif
