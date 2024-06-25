// this demonstrates rendering a triangle in Vulkan
#include "../include/engine.h"

int main() {
	int SCREEN = 512;
	// int frame = 0;

	InitParams params = {
		.flags = SDL_INIT_VIDEO,
		.title = "Vulkan Triangle",
		.width = SCREEN,
		.height = SCREEN,
	};

	const char *vertPath = "./examples/shaders/vkTriangle.vert.spv";
	const char *fragPath = "./examples/shaders/vkTriangle.frag.spv";

	VulkanVertex vertices[] = {
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	// float vertices[] = {
	// 	0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
	// 	0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
	// 	-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	// };

	VkVertexInputBindingDescription bindingDescription = getBindingDescription(sizeof(float) * 6);
	VkVertexInputAttributeDescription* attributeDescription = getAttributeDescriptions();

	VulkanEntity triangle = {
		.vertices = vertices,
		.verticesCount = 3,
		.verticesSizeof = sizeof(float) * 6,
		.bindingDescriptions = &bindingDescription,
		.attributeDescriptions = attributeDescription,
		.bindingDescriptionsCount = 1,
		.attributeDescriptionsCount = 2,
	};

	GameEngine engine = initVulkanEngine(params);
	VulkanEngine vulkan = initVulkan(
		engine.window,
		&triangle,
		params.title,
		vertPath,
		fragPath);

	SDL_Event e;
	char quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = 1; }
		}
		// vertices[1] = cos(frame * 0.01);
		drawFrame(&vulkan, &triangle);
		// frame++;
	}

	vkDeviceWaitIdle(vulkan.logicalDevice);
	deallocVulkanEntity(&triangle);
	deallocVulkan(&vulkan);
	dealloc(&engine);
	return 0;
}
