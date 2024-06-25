#pragma once

#include <vulkan/vulkan.h>
#ifdef _WIN32
#include <SDL.h>
#include <SDL_vulkan.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#endif

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VULKAN_DEBUG 1

#define MAX_FRAMES_IN_FLIGHT 2

// this one line is required to include the key:
// VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, or "VK_KHR_portability_subset"
// so if we don't include it, or if any problems arise, simply replace the
// macro with the string literal
#define VK_ENABLE_BETA_EXTENSIONS

// new methods
VulkanEngine initVulkan(
	SDL_Window *window,
	VulkanEntity *entity,
	const char *windowTitle,
	const char *vertPath,
	const char *fragPath);

void deallocVulkan(VulkanEngine *engine);

void deallocVulkanEntity(VulkanEntity *entity);

void vulkanCreateInstance(
	VulkanEngine *vulkan,
	SDL_Window *window,
	const char *windowTitle,
	const char **userExtensionNames,
	const unsigned int userExtensionsCount);

void vulkanCreateSurface(VulkanEngine *vulkan, SDL_Window *window);

void vulkanGetPhysicalDevice(VulkanEngine *vulkan);

void vulkanCreateLogicalDevice(
	VulkanEngine *vulkan,
	const char **deviceExtensions,
	unsigned int deviceExtensionsCount);

void vulkanCreateSwapChain(VulkanEngine *vulkan, SDL_Window *window);

void vulkanCreateImageViews(VulkanEngine *vulkan);

void vulkanCreateGraphicsPipeline(
	VulkanEngine *vulkan,
	VulkanEntity *entity,
	const char *vertPath,
	const char *fragPath);

void vulkanCreateRenderPass(VulkanEngine *vulkan);

void vulkanCreateFramebuffers(VulkanEngine *vulkan);

void vulkanCreateCommandPool(VulkanEngine *vulkan);

void vulkanCreateCommandBuffer(VulkanEngine *vulkan);

void vulkanCreateSyncObjects(VulkanEngine *vulkan);

#ifdef __cplusplus
}
#endif
