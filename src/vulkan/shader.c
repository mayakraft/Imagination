#include "shader.h"

uint32_t findMemoryType(
	VulkanEngine *vulkan,
	uint32_t typeFilter,
	VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(vulkan->physicalDevice, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	fputs("failed to find suitable memory type\n", stderr);
	return 0;
}

void vulkanCreateVertexBuffer(VulkanEngine *vulkan, VulkanEntity *entity) {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	// bufferInfo.size = sizeof(entity->vertices[0]) * vertices.size();
	bufferInfo.size = entity->verticesSizeof * entity->verticesCount;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult resultBuffer = vkCreateBuffer(
		vulkan->logicalDevice,
		&bufferInfo,
		NULL,
		&entity->vertexBuffer);
	if (resultBuffer != VK_SUCCESS) {
		fputs("failed to create vertex buffer\n", stderr);
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(
		vulkan->logicalDevice,
		entity->vertexBuffer,
		&memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(
		vulkan,
		memRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkResult resultAllocate = vkAllocateMemory(
		vulkan->logicalDevice,
		&allocInfo,
		NULL,
		&entity->vertexBufferMemory);
	if (resultAllocate != VK_SUCCESS) {
		fputs("failed to allocate vertex buffer memory\n", stderr);
	}

	vkBindBufferMemory(
		vulkan->logicalDevice,
		entity->vertexBuffer,
		entity->vertexBufferMemory,
		0);

	void* data;
	vkMapMemory(
		vulkan->logicalDevice,
		entity->vertexBufferMemory,
		0,
		bufferInfo.size,
		0,
		&data);
	memcpy(data, entity->vertices, (size_t) bufferInfo.size);
	vkUnmapMemory(vulkan->logicalDevice, entity->vertexBufferMemory);
}
