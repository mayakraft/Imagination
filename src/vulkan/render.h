#pragma once

#include "initialize.h"

#ifdef __cplusplus
extern "C" {
#endif

void drawFrame(VulkanEngine *engine, VulkanEntity *entity);

void recordCommandBuffer(
	VulkanEngine *engine,
	VulkanEntity *entity,
	VkCommandBuffer commandBuffer,
	uint32_t imageIndex);

#ifdef __cplusplus
}
#endif
