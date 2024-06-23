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

/**
 * Of the (potentially) many Vulkan-ready devices, we need to choose
 * the best fit device for our purposes, so, this is a heuristic
 * method, in which the needs may change from app to app.
 */
VkPhysicalDevice chooseBestDevice(VkPhysicalDevice* devices, unsigned int devicesCount) {
	for (unsigned int i = 0; i < devicesCount; i += 1) {
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
		vkGetPhysicalDeviceFeatures(devices[i], &deviceFeatures);
		// we don't want "other" or "cpu", anything else is okay.
		// if you want to require things, like fullDrawIndexUint32, list them here
		if (true
			// deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_OTHER
			// deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU
			&& (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
				|| deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
				|| deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
			// && deviceFeatures.robustBufferAccess
			// && deviceFeatures.shaderFloat64
			// && deviceFeatures.fullDrawIndexUint32
			) {
			return devices[i];
		}
	}
	fputs("No suitable Vulkan device found", stderr);
	return NULL; // I made this up. should it be -1?
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(VkSurfaceFormatKHR* availableFormats, unsigned int count) {
	for (unsigned int i = 0; i < count; i++) {
		VkSurfaceFormatKHR availableFormat = availableFormats[i];
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
			&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(VkPresentModeKHR* availablePresentModes, unsigned int count) {
	for (unsigned int i = 0; i < count; i++) {
		VkPresentModeKHR availablePresentMode = availablePresentModes[i];
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(SDL_Window *window, const VkSurfaceCapabilitiesKHR capabilities) {
	if (capabilities.currentExtent.width != 0xffffffff) {
		return capabilities.currentExtent;
	}

	// printf("capabilities min %d, %d\n", capabilities.minImageExtent.width, capabilities.minImageExtent.height);
	// printf("capabilities max %d, %d\n", capabilities.maxImageExtent.width, capabilities.maxImageExtent.height);
	// printf("capabilities current %d, %d\n", capabilities.currentExtent.width, capabilities.currentExtent.height);

	int width, height;
	SDL_Vulkan_GetDrawableSize(window, &width, &height);
	VkExtent2D actualExtent = { (unsigned int)width, (unsigned int)height };
	actualExtent.width = fmax(fmin(actualExtent.width, capabilities.maxImageExtent.width), capabilities.minImageExtent.width);
	actualExtent.height = fmax(fmin(actualExtent.height, capabilities.maxImageExtent.height), capabilities.minImageExtent.height);
	return actualExtent;
}

void initVulkan(SDL_Window *window, const char* appTitle) {
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
	VkApplicationInfo appInfo;
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
	VkInstanceCreateInfo instanceCreateInfo;
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
		VkDeviceQueueCreateInfo queueCreateInfo;
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = uniqueQueueFamilies[i];
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos[i] = queueCreateInfo;
	}

	// Device features include things like geometry shaders, we can require
	// these things here, as long as we detected them in the chooseBestDevice.
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

	// When we create the device, provide this struct.
	// Link the previous two structs, with count info, and set all others to 0.
	VkDeviceCreateInfo deviceCreateInfo;
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
		fputs("failed to create logical device!", stderr);
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

	VkSurfaceCapabilitiesKHR capabilities;
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
	VkSwapchainCreateInfoKHR swapCreateInfo;
	swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapCreateInfo.surface = surface;
	swapCreateInfo.minImageCount = imageCount;
	swapCreateInfo.imageFormat = surfaceFormat.format;
	swapCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapCreateInfo.imageExtent = extent;
	swapCreateInfo.imageArrayLayers = 1;
	swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	// if (indices.graphicsFamily != indices.presentFamily) {
	// 	swapCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	// 	swapCreateInfo.queueFamilyIndexCount = 2;
	// 	swapCreateInfo.pQueueFamilyIndices = swapQueueFamilyIndices;
	// } else {
	// 	swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	// 	swapCreateInfo.queueFamilyIndexCount = 0; // Optional
	// 	swapCreateInfo.pQueueFamilyIndices = NULL; // Optional
	// }
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
		VkImageViewCreateInfo createInfo;
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



	// cleanup
	// for (auto imageView : swapChainImageViews) {
	// 	vkDestroyImageView(logicalDevice, imageView, nullptr);
	// }
	// vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
	// vkDestroyDevice(logicalDevice, nullptr);
	// vkDestroySurfaceKHR(instance, surface, nullptr);
	// vkDestroyInstance(instance, nullptr);

	// free
	// VkSurfaceFormatKHR *formats;
	// VkPresentModeKHR *presentModes;
	// VkImage* swapChainImages;
	// VkImageView* swapChainImageViews;
}

int main() {
	int SCREEN = 512;
	// int frame = 0;

	InitParams params = {
		.flags = SDL_INIT_VIDEO,
		.title = "Vulkan Triangle",
		.width = SCREEN,
		.height = SCREEN,
	};

	GameEngine engine = initVulkanEngine(params);

	initVulkan(engine.window, params.title);

	SDL_Event e;
	char quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = 1; }
		}

		// frame += 1;
	}

	dealloc(&engine);
	return 0;
}
