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

typedef struct VulkanEngine {
	// we only need one instance. clean up upon application exit.
	VkInstance instance;
	// surface must be initialized before devices, it can affect device selection
	VkSurfaceKHR surface;
	// physical device will be implicitly cleaned up on its own.
	VkPhysicalDevice physicalDevice;
	// logical devices will need to be cleaned up, before the instance itself
	VkDevice logicalDevice;
	// device queues are implicitly cleaned up on their own.
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSwapchainKHR swapChain;
	VkImage* swapChainImages;
	VkImageView* swapChainImageViews;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkCommandPool commandPool;
	VkFramebuffer *swapChainFramebuffers;

	VkFence* inFlightFences;
	VkSemaphore* imageAvailableSemaphores;
	VkSemaphore* renderFinishedSemaphores;
	VkCommandBuffer* commandBuffers;

	unsigned int framebufferCount;
} VulkanEngine;

VulkanEngine initVulkan(SDL_Window *window, const char* windowTitle);

void recordCommandBuffer(VulkanEngine *engine, VkCommandBuffer commandBuffer, uint32_t imageIndex);

void drawFrame(VulkanEngine *engine);

void deallocVulkanEngine(VulkanEngine *engine);

#ifdef __cplusplus
}
#endif
