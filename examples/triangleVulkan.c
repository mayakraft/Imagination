// this demonstrates rendering a triangle in Vulkan
#include "../include/engine.h"

int main() {
	int SCREEN = 512;

	InitParams params = {
		.flags = SDL_INIT_VIDEO,
		.title = "Vulkan Triangle",
		.width = SCREEN,
		.height = SCREEN,
	};

	const char *vertPath = "./examples/shaders/vkTriangle.vert.spv";
	const char *fragPath = "./examples/shaders/vkTriangle.frag.spv";

	GameEngine engine = initVulkanEngine(params);
	VulkanEngine vulkan = initVulkan(engine.window, params.title, vertPath, fragPath);

	SDL_Event e;
	char quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) { quit = 1; }
		}
		drawFrame(&vulkan);
	}

	vkDeviceWaitIdle(vulkan.logicalDevice);
	deallocVulkan(&vulkan);
	dealloc(&engine);
	return 0;
}
