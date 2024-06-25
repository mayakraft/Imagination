#pragma once

#include "initialize.h"

#ifdef __cplusplus
extern "C" {
#endif

void drawFrame(VulkanEngine *engine);

void recordCommandBuffer(
	VulkanEngine *engine,
	VkCommandBuffer commandBuffer,
	uint32_t imageIndex);

#ifdef __cplusplus
}
#endif
