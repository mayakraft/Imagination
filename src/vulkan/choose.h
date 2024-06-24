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

VkPhysicalDevice chooseBestDevice(
	VkPhysicalDevice* devices,
	unsigned int count);

VkSurfaceFormatKHR chooseSwapSurfaceFormat(
	VkSurfaceFormatKHR* availableFormats,
	unsigned int count);

VkPresentModeKHR chooseSwapPresentMode(
	VkPresentModeKHR* availablePresentModes,
	unsigned int count);

VkExtent2D chooseSwapExtent(
	SDL_Window *window,
	const VkSurfaceCapabilitiesKHR capabilities);

#ifdef __cplusplus
}
#endif
