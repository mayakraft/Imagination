#include "choose.h"

/**
 * Of the (potentially) many Vulkan-ready devices, we need to choose
 * the best fit device for our purposes, so, this is a heuristic
 * method, in which the needs may change from app to app.
 */
VkPhysicalDevice chooseBestDevice(VkPhysicalDevice* devices, unsigned int count) {
	for (unsigned int i = 0; i < count; i += 1) {
		VkPhysicalDeviceProperties deviceProperties = {};
		VkPhysicalDeviceFeatures deviceFeatures = {};
		vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
		vkGetPhysicalDeviceFeatures(devices[i], &deviceFeatures);
		// we don't want "other" or "cpu", anything else is okay.
		// if you want to require things, like fullDrawIndexUint32, list them here
		if (1
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
