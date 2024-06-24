// this demonstrates rendering a triangle in Vulkan
#include "../include/engine.h"
#include "vulkan/vulkan_core.h"
#include <math.h>

// temporarily here. move to library when done
#include <vulkan/vulkan.h>
#ifdef _WIN32
#include <SDL.h>
#include <SDL_vulkan.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#endif

// this one line is required to include the key:
// VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, or "VK_KHR_portability_subset"
// so if we don't include it, or if any problems arise, simply replace the
// macro with the string literal
#define VK_ENABLE_BETA_EXTENSIONS

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
	VkFramebuffer *swapChainFramebuffers;

	VkPipeline graphicsPipeline;

	VkFence* inFlightFences;
	VkSemaphore* imageAvailableSemaphores;
	VkSemaphore* renderFinishedSemaphores;
	VkCommandBuffer* commandBuffers;
} VulkanEngine;

const int MAX_FRAMES_IN_FLIGHT = 2;
unsigned int currentFrame = 0;


VulkanEngine initVulkan(SDL_Window *window, const char* appTitle) {
	const char *vertPath = "./examples/shaders/vkTriangle.vert.spv";
	const char *fragPath = "./examples/shaders/vkTriangle.frag.spv";

	VkInstance instance;
	// physical device will be implicitly cleaned up on its own.
	VkPhysicalDevice physicalDevice = NULL;
	// surface must be initialized before devices, it can affect device selection
	VkSurfaceKHR surface;
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

	// pipeline
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	// frame buffers
	VkFramebuffer *swapChainFramebuffers;

	// commands
	VkCommandPool commandPool;
	VkCommandBuffer* commandBuffers;

	// synchronization objects
	VkSemaphore* imageAvailableSemaphores;
	VkSemaphore* renderFinishedSemaphores;
	VkFence* inFlightFences;





	//
	// user preferences
	//
	// these are user-requested extensions
	// see appendex [1]
	unsigned int userExtensionsCount = 1;
	const char* userExtensionNames[] = {
		VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
	};

	unsigned int deviceExtensionsCount = 1;
	const char* deviceExtensions[] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	//
	// Vulkan instance
	//
	// the Vulkan instance is a connection between our app and the Vulkan library
	//

	// we need to tell the instance some information about our app.
	// however, pretty much all of this info is optional.
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appTitle;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Vulkan Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// to initialize Vulkan, we need to specify which extensions to use,
	// because Vulkan is platform agnostic, these extensions inform Vulkan
	// about our window system specifically, in our case, SDL2.
	// SDL2 actually provides a helper function to generate these extensions.

	// Ask SDL2 to generate a list of extensions we will need for our app.
	// on my MacOS M1, this results in two: VK_KHR_surface, VK_EXT_metal_surface

	// these are extensions which are (available? required?) by the device
	unsigned int extensionCount = 0;
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, NULL);
	unsigned int totalExtensionCount = extensionCount + userExtensionsCount;
	const char* extensionNames[totalExtensionCount];
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensionNames);

	// append the user extensions
	for (unsigned int i = 0; i < userExtensionsCount; i++) {
		extensionNames[extensionCount + i] = userExtensionNames[i];
	}

	printf("Number of Vulkan Extensions: %d\n", totalExtensionCount);
	for (unsigned int i = 0; i < totalExtensionCount; i++) {
		printf("Extension: %s\n", extensionNames[i]);
	}

	// here is the struct we will pass into the create instance function
	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledExtensionCount = totalExtensionCount;
	instanceCreateInfo.ppEnabledExtensionNames = extensionNames;
	// we will come back to this layer count later.
	instanceCreateInfo.enabledLayerCount = 0;
	instanceCreateInfo.pNext = NULL;
	instanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

	// create the Vulkan instance. this concludes this section
	VkResult resultCreateInstance = vkCreateInstance(&instanceCreateInfo, NULL, &instance);
	if (resultCreateInstance != VK_SUCCESS) {
		printf("vkCreateInstance error: %d\n", resultCreateInstance);
		fputs("failed to create instance\n", stderr);
	}

	//
	// window surface
	//
	// To maintain platform agnosticism, Vulkan cannot interface with a window
	// system on its own. Ask GLFW to create this, specific to our platform.
	//
	SDL_bool resultSurface = SDL_Vulkan_CreateSurface(window, instance, &surface);
	if (resultSurface != 1) {
		fputs("failed to create window surface\n", stderr);
	}

	//
	// Vulkan physical device
	//
	// The physical device refers to the IRL physical hardware.
	// There may be multiple physical devices, in which case, we will want to
	// query the capabilities of each device.
	//

	// right from the start, we are able to query for all Vulkan-enabled devices
	unsigned int deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
	VkPhysicalDevice devices[deviceCount];
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
	if (deviceCount == 0) {
		fputs("failed to find GPUs with Vulkan support\n", stderr);
	}
	printf("Number of Devices: %d\n", deviceCount);

	// if there are more than one device, we need to somehow only choose 1.
	// we will need to write some kind of heuristic method to determine
	// the best option, or ask the user which they prefer.
	physicalDevice = chooseBestDevice(devices, deviceCount);

	//
	// Vulkan logical device
	//
	// The logical device is a digital interface to a physical device,
	// where we can setup which features we would like to interface with.
	//

	// different queues have the ability to perform different tasks like
	// graphics, encoding/decoding, computing, etc..
	// we need to get the graphics queue from our device, but before we search
	// for it, we have to know how many queues we iterate over.
	unsigned int queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(
		physicalDevice,
		&queueFamilyCount,
		NULL);
	VkQueueFamilyProperties queueFamilies[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(
		physicalDevice,
		&queueFamilyCount,
		queueFamilies);
	printf("queueFamilyCount: %d\n", queueFamilyCount);

	// iterate over the queues to find which is the graphics queue.
	unsigned int graphicsFamily;
	unsigned int presentFamily;
	unsigned char graphicsFamilyDidSet = 0;
	unsigned char presentFamilyDidSet = 0;
	for (unsigned int i = 0; i < queueFamilyCount; i += 1) {
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			graphicsFamily = i;
			graphicsFamilyDidSet = 1;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
		if (presentSupport) {
			presentFamily = i;
			presentFamilyDidSet = 1;
		}
	}

	// this process should be inside the "choose best device" functionality.
	if (graphicsFamilyDidSet == 0 || presentFamilyDidSet == 0) {
		fputs("selected GPU does not support correct queue families\n", stderr);
	}
	printf("graphicsFamily %d\n", graphicsFamily);
	printf("presentFamily %d\n", presentFamily);

	const int queueCreateInfosCount = 2;
	unsigned int uniqueQueueFamilies[queueCreateInfosCount] = { graphicsFamily, presentFamily };
	VkDeviceQueueCreateInfo queueCreateInfos[queueCreateInfosCount];

	// This struct specifies how many queues we want for this particular
	// queue family, in this case, the graphics queue family.
	float queuePriority = 1.0f;
	for (int i = 0; i < queueCreateInfosCount; i++) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = uniqueQueueFamilies[i];
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos[i] = queueCreateInfo;
	}

	// Device features include things like geometry shaders, we can require
	// these things here, as long as we detected them in the chooseBestDevice.
	VkPhysicalDeviceFeatures deviceFeatures = {};

	// When we create the device, provide this struct.
	// Link the previous two structs, with count info, and set all others to 0.
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.queueCreateInfoCount = queueCreateInfosCount;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.enabledExtensionCount = deviceExtensionsCount;
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions;

	VkResult resultCreateLogicalDevice = vkCreateDevice(
		physicalDevice,
		&deviceCreateInfo,
		NULL,
		&logicalDevice);

	printf("vkCreateDevice Logical: %d\n", resultCreateLogicalDevice);
	if (resultCreateLogicalDevice != VK_SUCCESS) {
		fputs("failed to create logical device\n", stderr);
	}

	// The Queue family stuff was previously calculated on the physical device.
	// Now that we have a logical device, we can get the actual graphics queue.
	// 0 is the index of the first (and only) queue, if we were creating
	// more than one we would increment the number here.
	vkGetDeviceQueue(logicalDevice, graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, presentFamily, 0, &presentQueue);

	//
	// Swap Chain
	//

	VkSurfaceCapabilitiesKHR capabilities = {};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

	VkSurfaceFormatKHR *formats = NULL;
	unsigned int formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);
	if (formatCount != 0) {
		formats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats);
	} else {
		fputs("no VkSurfaceFormatKHR", stderr);
	}

	VkPresentModeKHR *presentModes = NULL;
	unsigned int presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);
	if (presentModeCount != 0) {
		presentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes);
	} else {
		fputs("no VkPresentModeKHR", stderr);
	}

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(formats, formatCount);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(presentModes, presentModeCount);
	VkExtent2D extent = chooseSwapExtent(window, capabilities);
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	unsigned int imageCount = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0
		&& imageCount > capabilities.maxImageCount) {
		imageCount = capabilities.maxImageCount;
	}

	printf("formatCount %d\n", formatCount);
	printf("presentModeCount %d\n", presentModeCount);
	printf("capabilities.currentExtent %d %d\n", capabilities.currentExtent.width, capabilities.currentExtent.height);
	printf("surfaceFormat.format %d\n", surfaceFormat.format);
	printf("imageCount %d\n", imageCount);

	unsigned int swapQueueFamilyIndices[] = { graphicsFamily, presentFamily };
	VkSwapchainCreateInfoKHR swapCreateInfo = {};
	swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapCreateInfo.surface = surface;
	swapCreateInfo.minImageCount = imageCount;
	swapCreateInfo.imageFormat = surfaceFormat.format;
	swapCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapCreateInfo.imageExtent = extent;
	swapCreateInfo.imageArrayLayers = 1;
	swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	swapCreateInfo.queueFamilyIndexCount = 2;
	swapCreateInfo.pQueueFamilyIndices = swapQueueFamilyIndices;
	swapCreateInfo.preTransform = capabilities.currentTransform;
	swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapCreateInfo.presentMode = presentMode;
	swapCreateInfo.clipped = VK_TRUE;
	swapCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult resultSwap = vkCreateSwapchainKHR(logicalDevice, &swapCreateInfo, NULL, &swapChain);
	if (resultSwap != VK_SUCCESS) {
		printf("vkCreateSwapchainKHR error: %d\n", resultSwap);
		fputs("failed to create swap chain\n", stderr);
	}

	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, NULL);
	swapChainImages = (VkImage*)malloc(sizeof(VkImage) * imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages);

	//
	// Swap Chain Image Views
	//

	swapChainImageViews = (VkImageView*)malloc(sizeof(VkImageView) * imageCount);
	for (size_t i = 0; i < imageCount; i++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		if (vkCreateImageView(logicalDevice, &createInfo, NULL, &swapChainImageViews[i]) != VK_SUCCESS) {
			fputs("failed to create image views\n", stderr);
		}
	}

	//
	// Render Pass
	//

	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachment;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(logicalDevice, &renderPassCreateInfo, NULL, &renderPass) != VK_SUCCESS) {
		fputs("failed to create render pass\n", stderr);
	}


	//
	// Shaders
	//

	void* vertCode = readFile(vertPath);
	void* fragCode = readFile(fragPath);
	unsigned int vertCodeSize = sizeof(vertCode);
	unsigned int fragCodeSize = sizeof(fragCode);
	printf("vertex %d:\n", vertCodeSize);
	// for (int i = 0; i < vertCodeSize; i++) printf("%u", vertCode[i]);
	printf("\n");
	printf("fragment %d:\n", fragCodeSize);
	// for (int i = 0; i < fragCodeSize; i++) printf("%u", fragCode[i]);
	printf("\n");

	VkShaderModuleCreateInfo vertCreateInfo = {};
	vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertCreateInfo.codeSize = 1504; // sizeof(vertCode);
	vertCreateInfo.pCode = (const unsigned int*)(vertCode);
	VkShaderModule vertShaderModule;
	if (vkCreateShaderModule(logicalDevice, &vertCreateInfo, NULL, &vertShaderModule) != VK_SUCCESS) {
		fputs("failed to create shader module\n", stderr);
	}

	VkShaderModuleCreateInfo fragCreateInfo = {};
	fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragCreateInfo.codeSize = 572; // sizeof(fragCode);
	fragCreateInfo.pCode = (const unsigned int*)(fragCode);
	VkShaderModule fragShaderModule;
	if (vkCreateShaderModule(logicalDevice, &fragCreateInfo, NULL, &fragShaderModule) != VK_SUCCESS) {
		fputs("failed to create shader module\n", stderr);
	}



	//
	// Pipeline
	//

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {
		vertShaderStageInfo,
		fragShaderStageInfo
	};

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = NULL; // Optional
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = NULL; // Optional

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	//
	// Pipeline: Viewport and Scissors
	//

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) swapChainExtent.width;
	viewport.height = (float) swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	VkOffset2D scissorOffset = { 0, 0 };
	scissor.offset = scissorOffset;
	scissor.extent = swapChainExtent;

	unsigned int dynamicStatesCount = 2;
	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = dynamicStatesCount;
	dynamicState.pDynamicStates = dynamicStates;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	//
	// Pipeline: Rasterizer
	//

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = NULL; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
		| VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT
		| VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = NULL; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = NULL; // Optional

	if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
		fputs("failed to create pipeline layout\n", stderr);
	}

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = NULL; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	VkResult resultPipeline = vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline);
	if (resultPipeline != VK_SUCCESS) {
		printf("vkCreateGraphicsPipelines error: %d\n", resultPipeline);
		fputs("failed to create graphics pipeline\n", stderr);
	}

	//
	// Framebuffers
	//

	// size of swapChainImageViews = imageCount
	swapChainFramebuffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * imageCount);
	// swapChainFramebuffers.resize(swapChainImageViews.size());
	for (size_t i = 0; i < imageCount; i++) {
		VkImageView attachments[] = { swapChainImageViews[i] };
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;
		if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, NULL, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			fputs("failed to create framebuffer\n", stderr);
		}
	}


	//
	// Commands
	//

	commandBuffers = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer) * MAX_FRAMES_IN_FLIGHT);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = graphicsFamily;
	if (vkCreateCommandPool(logicalDevice, &poolInfo, NULL, &commandPool) != VK_SUCCESS) {
		fputs("failed to create command pool\n", stderr);
	}

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (unsigned int)MAX_FRAMES_IN_FLIGHT;
	if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers) != VK_SUCCESS) {
		fputs("failed to allocate command buffers\n", stderr);
	}

	//
	// Synchronization objects
	//

	imageAvailableSemaphores = (VkSemaphore*)malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores = (VkSemaphore*)malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
	inFlightFences = (VkFence*)malloc(sizeof(VkFence) * MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, NULL, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
		vkCreateSemaphore(logicalDevice, &semaphoreInfo, NULL, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
		vkCreateFence(logicalDevice, &fenceInfo, NULL, &inFlightFences[i]) != VK_SUCCESS) {
			fputs("failed to create synchronization objects for a frame\n", stderr);
		}
	}

	// if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, NULL, &imageAvailableSemaphore) != VK_SUCCESS ||
	// 	vkCreateSemaphore(logicalDevice, &semaphoreInfo, NULL, &renderFinishedSemaphore) != VK_SUCCESS ||
	// 	vkCreateFence(logicalDevice, &fenceInfo, NULL, &inFlightFence) != VK_SUCCESS) {
	// 	throw std::runtime_error("failed to create semaphores!");
	// }

	printf("formatCount %d\n", formatCount);
	printf("presentModeCount %d\n", presentModeCount);
	printf("capabilities.currentExtent %d %d\n", capabilities.currentExtent.width,  capabilities.currentExtent.height);
	printf("surfaceFormat.format %d\n", surfaceFormat.format);
	printf("imageCount %d\n", imageCount);

	//
	// Cleanup inside method
	//

	// Vulkan
	vkDestroyShaderModule(logicalDevice, fragShaderModule, NULL);
	vkDestroyShaderModule(logicalDevice, vertShaderModule, NULL);

	VulkanEngine engine = {
		// we only need one instance. clean up upon application exit.
		.instance = instance,
		// surface must be initialized before devices, it can affect device selection
		.surface = surface,
		// physical device will be implicitly cleaned up on its own.
		.physicalDevice = physicalDevice,
		// logical devices will need to be cleaned up, before the instance itself
		.logicalDevice = logicalDevice,
		// device queues are implicitly cleaned up on their own.
		.graphicsQueue = graphicsQueue,
		.presentQueue = presentQueue,

		.swapChain = swapChain,
		.swapChainImages = swapChainImages,
		.swapChainImageViews = swapChainImageViews,
		.swapChainImageFormat = swapChainImageFormat,
		.swapChainExtent = swapChainExtent,

		.renderPass = renderPass,
		.swapChainFramebuffers = swapChainFramebuffers,
		.graphicsPipeline = graphicsPipeline,

		.inFlightFences = inFlightFences,
		.imageAvailableSemaphores = imageAvailableSemaphores,
		.commandBuffers = commandBuffers,
		.renderFinishedSemaphores = renderFinishedSemaphores
	};
	return engine;
}

void deallocVulkanEngine(VulkanEngine *engine) {
	// for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
	// 	vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], NULL);
	// 	vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], NULL);
	// 	vkDestroyFence(logicalDevice, inFlightFences[i], NULL);
	// }
	// vkDestroyCommandPool(logicalDevice, commandPool, NULL);
	// for (auto framebuffer : swapChainFramebuffers) {
	// 	vkDestroyFramebuffer(logicalDevice, framebuffer, NULL);
	// }
	// vkDestroyPipeline(logicalDevice, graphicsPipeline, NULL);
	// vkDestroyPipelineLayout(logicalDevice, pipelineLayout, NULL);
	// vkDestroyRenderPass(logicalDevice, renderPass, NULL);
	// for (auto imageView : swapChainImageViews) {
	// 	vkDestroyImageView(logicalDevice, imageView, NULL);
	// }
	// vkDestroySwapchainKHR(logicalDevice, swapChain, NULL);
	// vkDestroyDevice(logicalDevice, NULL);
	// vkDestroySurfaceKHR(instance, surface, NULL);
	// vkDestroyInstance(instance, NULL);
}

void recordCommandBuffer(VulkanEngine *engine, VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = NULL; // Optional

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		fputs("failed to begin recording command buffer\n", stderr);
	}

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = engine->renderPass;
	renderPassBeginInfo.framebuffer = engine->swapChainFramebuffers[imageIndex];
	VkOffset2D renderAreaOffset = { 0, 0 };
	renderPassBeginInfo.renderArea.offset = renderAreaOffset;
	renderPassBeginInfo.renderArea.extent = engine->swapChainExtent;

	VkClearValue clearColor = {{{ 0.0f, 0.0f, 0.0f, 1.0f }}};
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, engine->graphicsPipeline);

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)(engine->swapChainExtent.width);
	viewport.height = (float)(engine->swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor = {};
	VkOffset2D scissorOffset = { 0, 0 };
	scissor.offset = scissorOffset;
	scissor.extent = engine->swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		fputs("failed to record command buffer\n", stderr);
	}
}

void drawFrame(VulkanEngine *engine) {
	vkWaitForFences(engine->logicalDevice, 1, &engine->inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(engine->logicalDevice, 1, &engine->inFlightFences[currentFrame]);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(engine->logicalDevice, engine->swapChain, UINT64_MAX, engine->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	vkResetCommandBuffer(engine->commandBuffers[currentFrame],  0);
	recordCommandBuffer(engine, engine->commandBuffers[currentFrame], imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {engine->imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &engine->commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = { engine->renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(engine->graphicsQueue, 1, &submitInfo, engine->inFlightFences[currentFrame]) != VK_SUCCESS) {
		fputs("failed to submit draw command buffer\n", stderr);
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { engine->swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	presentInfo.pResults = NULL; // Optional

	vkQueuePresentKHR(engine->presentQueue, &presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

int main() {
	int SCREEN = 512;

	InitParams params = {
		.flags = SDL_INIT_VIDEO,
		.title = "Vulkan Triangle",
		.width = SCREEN,
		.height = SCREEN,
	};

	GameEngine engine = initVulkanEngine(params);

	VulkanEngine vkEngine = initVulkan(engine.window, params.title);

	SDL_Event e;
	char quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = 1; }
		}
		drawFrame(&vkEngine);
	}
	vkDeviceWaitIdle(vkEngine.logicalDevice);

	// cleanup
	// for (auto imageView : vkEngine.swapChainImageViews) {
	// 	vkDestroyImageView(vkEngine.logicalDevice, imageView, NULL);
	// }
	vkDestroySwapchainKHR(vkEngine.logicalDevice, vkEngine.swapChain, NULL);
	vkDestroyDevice(vkEngine.logicalDevice, NULL);
	vkDestroySurfaceKHR(vkEngine.instance, vkEngine.surface, NULL);
	vkDestroyInstance(vkEngine.instance, NULL);

	// // free
	// VkSurfaceFormatKHR *formats;
	// VkPresentModeKHR *presentModes;
	// VkImage* swapChainImages;
	// VkImageView* swapChainImageViews;
	// swapChainFramebuffers

	dealloc(&engine);
	return 0;
}
