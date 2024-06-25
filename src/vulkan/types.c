#include "types.h"
#include <stdlib.h>

// for stride do something like; sizeof(float) * 6;
VkVertexInputBindingDescription getBindingDescription(unsigned int stride) {
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = stride;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescription;
}

VkVertexInputAttributeDescription* getAttributeDescriptions() {
	VkVertexInputAttributeDescription* descriptions = (VkVertexInputAttributeDescription*)malloc(sizeof(VkVertexInputAttributeDescription) * 2);
	// descriptions[0] = {};
	descriptions[0].binding = 0;
	descriptions[0].location = 0;
	descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	descriptions[0].offset = 0;
	// descriptions[0].offset = offsetof(Vertex, pos);

	// descriptions[1] = {};
	descriptions[1].binding = 0;
	descriptions[1].location = 1;
	descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	descriptions[1].offset = sizeof(float) * 3;
	// descriptions[1].offset = offsetof(Vertex, color);
	return descriptions;
}
