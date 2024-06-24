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
	deallocVulkanEngine(&vkEngine);
	dealloc(&engine);
	return 0;
}
