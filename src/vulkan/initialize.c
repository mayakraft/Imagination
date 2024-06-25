#include "initialize.h"
#include "choose.h"
#include "../core/fs.h"

VulkanEngine initVulkan(
	SDL_Window *window,
	const char *windowTitle,
	const char *vertPath,
	const char *fragPath
) {
	VulkanEngine vulkan = {};

	vulkan.currentFrame = 0;

	// add any additional extensions here to be added to ppEnabledExtensionNames.
	// the first in this list is required for MacOS (MoltenVK)
	// see https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Instance
	const unsigned int extCount = 1;
	const char *extNames[extCount] = {
		VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
	};

	unsigned int deviceExtensionsCount = 1;
	const char *deviceExtensions[] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	vulkanCreateInstance(&vulkan, window, windowTitle, extNames, extCount);
	vulkanCreateSurface(&vulkan, window);
	vulkanGetPhysicalDevice(&vulkan);
	vulkanCreateLogicalDevice(&vulkan, deviceExtensions, deviceExtensionsCount);
	vulkanCreateSwapChain(&vulkan, window);
	vulkanCreateImageViews(&vulkan);
	vulkanCreateRenderPass(&vulkan);
	vulkanCreateGraphicsPipeline(&vulkan, vertPath, fragPath);
	vulkanCreateFramebuffers(&vulkan);
	vulkanCreateCommandPool(&vulkan);
	vulkanCreateCommandBuffer(&vulkan);
	vulkanCreateSyncObjects(&vulkan);

	// cleanup
	vkDestroyShaderModule(vulkan.logicalDevice, vulkan.fragmentShaderModule, NULL);
	vkDestroyShaderModule(vulkan.logicalDevice, vulkan.vertexShaderModule, NULL);

	return vulkan;
}

//
// Vulkan instance
//
// the Vulkan instance is a connection between our app and the Vulkan library
//
void vulkanCreateInstance(
	VulkanEngine *vulkan,
	SDL_Window *window,
	const char *windowTitle,
	const char **userExtensionNames,
	const unsigned int userExtensionsCount
) {
	// we need to tell the instance some information about our app.
	// however, pretty much all of this info is optional.
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = windowTitle;
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
	VkResult resultCreateInstance = vkCreateInstance(
		&instanceCreateInfo,
		NULL,
		&vulkan->instance);
	if (resultCreateInstance != VK_SUCCESS) {
		fputs("failed to create instance\n", stderr);
	}

	if (VULKAN_DEBUG) {
		printf("Number of Vulkan Extensions: %d\n", totalExtensionCount);
		for (unsigned int i = 0; i < totalExtensionCount; i++) {
			printf("Extension: %s\n", extensionNames[i]);
		}
		if (resultCreateInstance != VK_SUCCESS) {
			printf("vkCreateInstance error: %d\n", resultCreateInstance);
		}
	}
}

//
// window surface
//
// To maintain platform agnosticism, Vulkan cannot interface with a window
// system on its own. Ask GLFW to create this, specific to our platform.
//
void vulkanCreateSurface(VulkanEngine *vulkan, SDL_Window *window) {
	SDL_bool resultSurface = SDL_Vulkan_CreateSurface(
		window,
		vulkan->instance,
		&vulkan->surface);
	if (resultSurface != 1) {
		fputs("failed to create window surface\n", stderr);
	}
}

//
// Vulkan physical device
//
// The physical device refers to the IRL physical hardware.
// There may be multiple physical devices, in which case, we will want to
// query the capabilities of each device.
//
void vulkanGetPhysicalDevice(VulkanEngine *vulkan) {
	// right from the start, we are able to query for all Vulkan-enabled devices
	unsigned int deviceCount = 0;
	vkEnumeratePhysicalDevices(vulkan->instance, &deviceCount, NULL);
	VkPhysicalDevice devices[deviceCount];
	vkEnumeratePhysicalDevices(vulkan->instance, &deviceCount, devices);
	if (deviceCount == 0) {
		fputs("failed to find GPUs with Vulkan support\n", stderr);
	}
	// if there are more than one device, we need to somehow only choose 1.
	// we will need to write some kind of heuristic method to determine
	// the best option, or ask the user which they prefer.
	vulkan->physicalDevice = vulkanChooseBestDeviceFromDevices(devices, deviceCount);

	if (VULKAN_DEBUG) {
		printf("Number of Devices: %d\n", deviceCount);
	}
}

//
// Vulkan logical device
//
// The logical device is a digital interface to a physical device,
// where we can setup which features we would like to interface with.
//
void vulkanCreateLogicalDevice(
	VulkanEngine *vulkan,
	const char **deviceExtensions,
	unsigned int deviceExtensionsCount) {
	// different queues have the ability to perform different tasks like
	// graphics, encoding/decoding, computing, etc..
	// we need to get the graphics queue from our device, but before we search
	// for it, we have to know how many queues we iterate over.
	unsigned int queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(
		vulkan->physicalDevice,
		&queueFamilyCount,
		NULL);
	VkQueueFamilyProperties queueFamilies[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(
		vulkan->physicalDevice,
		&queueFamilyCount,
		queueFamilies);

	// iterate over the queues to find which is the graphics queue.
	unsigned char graphicsFamilyDidSet = 0;
	unsigned char presentFamilyDidSet = 0;
	for (unsigned int i = 0; i < queueFamilyCount; i += 1) {
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			vulkan->graphicsFamily = i;
			graphicsFamilyDidSet = 1;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(
			vulkan->physicalDevice,
			i,
			vulkan->surface,
			&presentSupport);
		if (presentSupport) {
			vulkan->presentFamily = i;
			presentFamilyDidSet = 1;
		}
	}

	// this process should be inside the "choose best device" functionality.
	if (graphicsFamilyDidSet == 0 || presentFamilyDidSet == 0) {
		fputs("selected GPU does not support correct queue families\n", stderr);
	}

	const int queueCreateInfosCount = 2;
	unsigned int uniqueQueueFamilies[queueCreateInfosCount] = {
		vulkan->graphicsFamily,
		vulkan->presentFamily
	};
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
	// these things here, as long as we detected them in the vulkanChooseBestDevice.
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
		vulkan->physicalDevice,
		&deviceCreateInfo,
		NULL,
		&vulkan->logicalDevice);

	if (resultCreateLogicalDevice != VK_SUCCESS) {
		fputs("failed to create logical device\n", stderr);
	}

	// The Queue family stuff was previously calculated on the physical device.
	// Now that we have a logical device, we can get the actual graphics queue.
	// 0 is the index of the first (and only) queue, if we were creating
	// more than one we would increment the number here.
	vkGetDeviceQueue(
		vulkan->logicalDevice,
		vulkan->graphicsFamily,
		0,
		&vulkan->graphicsQueue);
	vkGetDeviceQueue(
		vulkan->logicalDevice,
		vulkan->presentFamily,
		0,
		&vulkan->presentQueue);

	if (VULKAN_DEBUG) {
		printf("queueFamilyCount: %d\n", queueFamilyCount);
		printf("graphicsFamily %d\n", vulkan->graphicsFamily);
		printf("presentFamily %d\n", vulkan->presentFamily);
		printf("vkCreateDevice Logical: %d\n", resultCreateLogicalDevice);
	}
}

//
// Swap Chain
//
void vulkanCreateSwapChain(VulkanEngine *vulkan, SDL_Window *window) {
	VkSurfaceCapabilitiesKHR capabilities = {};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		vulkan->physicalDevice,
		vulkan->surface,
		&capabilities);

	// this will be freed just below here
	VkSurfaceFormatKHR *formats = NULL;
	unsigned int formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(
		vulkan->physicalDevice,
		vulkan->surface,
		&formatCount,
		NULL);
	if (formatCount != 0) {
		formats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			vulkan->physicalDevice,
			vulkan->surface,
			&formatCount,
			formats);
	} else {
		fputs("no VkSurfaceFormatKHR", stderr);
	}

	// this will be freed just below here
	VkPresentModeKHR *presentModes = NULL;
	unsigned int presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(
		vulkan->physicalDevice,
		vulkan->surface,
		&presentModeCount,
		NULL);
	if (presentModeCount != 0) {
		presentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			vulkan->physicalDevice,
			vulkan->surface,
			&presentModeCount,
			presentModes);
	} else {
		fputs("no VkPresentModeKHR", stderr);
	}

	VkSurfaceFormatKHR surfaceFormat = vulkanChooseSwapSurfaceFormat(formats, formatCount);
	VkPresentModeKHR presentMode = vulkanChooseSwapPresentMode(presentModes, presentModeCount);
	VkExtent2D extent = vulkanChooseSwapExtent(window, capabilities);
	vulkan->swapChainImageFormat = surfaceFormat.format;
	vulkan->swapChainExtent = extent;

	free(formats);
	free(presentModes);

	vulkan->swapchainImageCount = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0
		&& vulkan->swapchainImageCount > capabilities.maxImageCount) {
		vulkan->swapchainImageCount = capabilities.maxImageCount;
	}

	unsigned int swapQueueFamilyIndices[] = {
		vulkan->graphicsFamily,
		vulkan->presentFamily
	};

	VkSwapchainCreateInfoKHR swapCreateInfo = {};
	swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapCreateInfo.surface = vulkan->surface;
	swapCreateInfo.minImageCount = vulkan->swapchainImageCount;
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

	VkResult resultSwap = vkCreateSwapchainKHR(
		vulkan->logicalDevice,
		&swapCreateInfo,
		NULL,
		&vulkan->swapChain);
	if (resultSwap != VK_SUCCESS) {
		fputs("failed to create swap chain\n", stderr);
	}

	vkGetSwapchainImagesKHR(
		vulkan->logicalDevice,
		vulkan->swapChain,
		&vulkan->swapchainImageCount,
		NULL);
	vulkan->swapChainImages = (VkImage*)malloc(sizeof(VkImage) * vulkan->swapchainImageCount);
	vkGetSwapchainImagesKHR(
		vulkan->logicalDevice,
		vulkan->swapChain,
		&vulkan->swapchainImageCount,
		vulkan->swapChainImages);

	if (VULKAN_DEBUG) {
		printf("formatCount %d\n", formatCount);
		printf("presentModeCount %d\n", presentModeCount);
		printf("capabilities.currentExtent %d %d\n", capabilities.currentExtent.width, capabilities.currentExtent.height);
		printf("surfaceFormat.format %d\n", surfaceFormat.format);
		printf("swapchainImageCount %d\n", vulkan->swapchainImageCount);
		if (resultSwap != VK_SUCCESS) {
			printf("vkCreateSwapchainKHR error: %d\n", resultSwap);
		}
	}
}


//
// Swap Chain Image Views
//
void vulkanCreateImageViews(VulkanEngine *vulkan) {
	vulkan->swapChainImageViews = (VkImageView*)malloc(sizeof(VkImageView) * vulkan->swapchainImageCount);

	for (size_t i = 0; i < vulkan->swapchainImageCount; i++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = vulkan->swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = vulkan->swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkResult resultImageViews = vkCreateImageView(
			vulkan->logicalDevice,
			&createInfo,
			NULL,
			&vulkan->swapChainImageViews[i]);
		if (resultImageViews != VK_SUCCESS) {
			fputs("failed to create image views\n", stderr);
		}
	}
}

//
// Shaders
//
void vulkanCreateGraphicsPipeline(
	VulkanEngine *vulkan,
	const char *vertPath,
	const char *fragPath
) {
	long vertCodeSize;
	long fragCodeSize;
	void* vertCode = readFile(vertPath, &vertCodeSize);
	void* fragCode = readFile(fragPath, &fragCodeSize);

	VkShaderModuleCreateInfo vertCreateInfo = {};
	vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertCreateInfo.codeSize = vertCodeSize;
	vertCreateInfo.pCode = (const unsigned int*)(vertCode);

	VkResult resultVertexShaderModule = vkCreateShaderModule(
		vulkan->logicalDevice,
		&vertCreateInfo,
		NULL,
		&vulkan->vertexShaderModule);
	if (resultVertexShaderModule != VK_SUCCESS) {
		fputs("failed to create shader module\n", stderr);
	}

	VkShaderModuleCreateInfo fragCreateInfo = {};
	fragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragCreateInfo.codeSize = fragCodeSize;
	fragCreateInfo.pCode = (const unsigned int*)(fragCode);

	VkResult resultFragmentShaderModule = vkCreateShaderModule(
		vulkan->logicalDevice,
		&fragCreateInfo,
		NULL,
		&vulkan->fragmentShaderModule);
	if (resultFragmentShaderModule != VK_SUCCESS) {
		fputs("failed to create shader module\n", stderr);
	}

	//
	// Pipeline
	//

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vulkan->vertexShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = vulkan->fragmentShaderModule;
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
	viewport.width = vulkan->swapChainExtent.width;
	viewport.height = vulkan->swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	VkOffset2D scissorOffset = { 0, 0 };
	scissor.offset = scissorOffset;
	scissor.extent = vulkan->swapChainExtent;

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

	VkResult resultPipelineLayout = vkCreatePipelineLayout(
		vulkan->logicalDevice,
		&pipelineLayoutInfo,
		NULL,
		&vulkan->pipelineLayout);
	if (resultPipelineLayout != VK_SUCCESS) {
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
	pipelineInfo.layout = vulkan->pipelineLayout;
	pipelineInfo.renderPass = vulkan->renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	VkResult resultPipeline = vkCreateGraphicsPipelines(
		vulkan->logicalDevice,
		VK_NULL_HANDLE,
		1,
		&pipelineInfo,
		NULL,
		&vulkan->graphicsPipeline);
	if (resultPipeline != VK_SUCCESS) {
		fputs("failed to create graphics pipeline\n", stderr);
	}
	if (VULKAN_DEBUG) {
		if (resultPipeline != VK_SUCCESS) {
			printf("vkCreateGraphicsPipelines error: %d\n", resultPipeline);
		}
	}
}

//
// Render Pass
//
void vulkanCreateRenderPass(VulkanEngine *vulkan) {
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = vulkan->swapChainImageFormat;
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

	VkResult resultRenderPass = vkCreateRenderPass(
		vulkan->logicalDevice,
		&renderPassCreateInfo,
		NULL,
		&vulkan->renderPass);
	if (resultRenderPass != VK_SUCCESS) {
		fputs("failed to create render pass\n", stderr);
	}
}

//
// Framebuffers
//
void vulkanCreateFramebuffers(VulkanEngine *vulkan) {
	// size of swapChainImageViews = swapchainImageCount
	vulkan->swapChainFramebuffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * vulkan->swapchainImageCount);
	// swapChainFramebuffers.resize(swapChainImageViews.size());
	for (size_t i = 0; i < vulkan->swapchainImageCount; i++) {
		VkImageView attachments[] = { vulkan->swapChainImageViews[i] };
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = vulkan->renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = vulkan->swapChainExtent.width;
		framebufferInfo.height = vulkan->swapChainExtent.height;
		framebufferInfo.layers = 1;
		VkResult resultFrameBuffer = vkCreateFramebuffer(
			vulkan->logicalDevice,
			&framebufferInfo,
			NULL,
			&vulkan->swapChainFramebuffers[i]);
		if (resultFrameBuffer != VK_SUCCESS) {
			fputs("failed to create framebuffer\n", stderr);
		}
	}
}

//
// Commands
//
void vulkanCreateCommandPool(VulkanEngine *vulkan) {
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = vulkan->graphicsFamily;
	if (vkCreateCommandPool(vulkan->logicalDevice, &poolInfo, NULL, &vulkan->commandPool) != VK_SUCCESS) {
		fputs("failed to create command pool\n", stderr);
	}
}

void vulkanCreateCommandBuffer(VulkanEngine *vulkan) {
	vulkan->commandBuffers = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer) * MAX_FRAMES_IN_FLIGHT);
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vulkan->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (unsigned int)MAX_FRAMES_IN_FLIGHT;
	if (vkAllocateCommandBuffers(vulkan->logicalDevice, &allocInfo, vulkan->commandBuffers) != VK_SUCCESS) {
		fputs("failed to allocate command buffers\n", stderr);
	}
}

//
// Synchronization objects
//
void vulkanCreateSyncObjects(VulkanEngine *vulkan) {
	vulkan->imageAvailableSemaphores = (VkSemaphore*)malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
	vulkan->renderFinishedSemaphores = (VkSemaphore*)malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
	vulkan->inFlightFences = (VkFence*)malloc(sizeof(VkFence) * MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(vulkan->logicalDevice, &semaphoreInfo, NULL, &vulkan->imageAvailableSemaphores[i]) != VK_SUCCESS ||
		vkCreateSemaphore(vulkan->logicalDevice, &semaphoreInfo, NULL, &vulkan->renderFinishedSemaphores[i]) != VK_SUCCESS ||
		vkCreateFence(vulkan->logicalDevice, &fenceInfo, NULL, &vulkan->inFlightFences[i]) != VK_SUCCESS) {
			fputs("failed to create synchronization objects for a frame\n", stderr);
		}
	}

	if (VULKAN_DEBUG) {
		printf("swapchainImageCount %d\n", vulkan->swapchainImageCount);
	}
}

void deallocVulkan(VulkanEngine *engine) {
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(engine->logicalDevice, engine->renderFinishedSemaphores[i], NULL);
		vkDestroySemaphore(engine->logicalDevice, engine->imageAvailableSemaphores[i], NULL);
		vkDestroyFence(engine->logicalDevice, engine->inFlightFences[i], NULL);
	}
	vkDestroyCommandPool(engine->logicalDevice, engine->commandPool, NULL);
	for (unsigned int i = 0; i < engine->framebufferCount; i++) {
		vkDestroyFramebuffer(engine->logicalDevice, engine->swapChainFramebuffers[i], NULL);
	}
	vkDestroyPipeline(engine->logicalDevice, engine->graphicsPipeline, NULL);
	vkDestroyPipelineLayout(engine->logicalDevice, engine->pipelineLayout, NULL);
	vkDestroyRenderPass(engine->logicalDevice, engine->renderPass, NULL);
	for (unsigned int i = 0; i < engine->framebufferCount; i++) {
		vkDestroyImageView(engine->logicalDevice, engine->swapChainImageViews[i], NULL);
	}
	vkDestroySwapchainKHR(engine->logicalDevice, engine->swapChain, NULL);
	vkDestroyDevice(engine->logicalDevice, NULL);
	vkDestroySurfaceKHR(engine->instance, engine->surface, NULL);
	vkDestroyInstance(engine->instance, NULL);

	// everything that was malloc()'d, except these two they were already freed.
	// VkSurfaceFormatKHR* formats;
	// VkPresentModeKHR* presentModes;
	free(engine->swapChainImages);
	free(engine->swapChainImageViews);
	free(engine->swapChainFramebuffers);
	free(engine->commandBuffers);
	free(engine->imageAvailableSemaphores);
	free(engine->renderFinishedSemaphores);
	free(engine->inFlightFences);
}
