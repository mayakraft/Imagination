#pragma once

#include <vulkan/vulkan.h>

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

	unsigned int graphicsFamily;
	unsigned int presentFamily;

	VkSwapchainKHR swapChain;
	VkImage* swapChainImages;
	VkImageView* swapChainImageViews;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	unsigned int swapchainImageCount;
	VkShaderModule vertexShaderModule;
	VkShaderModule fragmentShaderModule;

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

	unsigned int currentFrame;
} VulkanEngine;

typedef struct VulkanVertex {
	float pos[3];
	float color[3];
} VulkanVertex;

typedef struct VulkanEntity {
	VulkanVertex *vertices;
	unsigned int verticesCount;
	unsigned int verticesSizeof;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkVertexInputBindingDescription *bindingDescriptions;
	VkVertexInputAttributeDescription *attributeDescriptions;
	unsigned int bindingDescriptionsCount;
	unsigned int attributeDescriptionsCount;
} VulkanEntity;

// for stride do something like; sizeof(float) * 6;
VkVertexInputBindingDescription getBindingDescription(unsigned int stride);

VkVertexInputAttributeDescription* getAttributeDescriptions();

#ifdef __cplusplus
}
#endif
