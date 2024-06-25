#include "render.h"

void recordCommandBuffer(
	VulkanEngine *engine,
	VulkanEntity *entity,
	VkCommandBuffer commandBuffer,
	uint32_t imageIndex
) {
	VkClearValue clearColor = {{{ 0.0f, 0.0f, 0.0f, 1.0f }}};

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = NULL; // Optional

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		fputs("failed to begin recording command buffer\n", stderr);
	}

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = engine->renderPass;
	renderPassBeginInfo.framebuffer = engine->swapChainFramebuffers[imageIndex];
	VkOffset2D renderAreaOffset = { 0, 0 };
	renderPassBeginInfo.renderArea.offset = renderAreaOffset;
	renderPassBeginInfo.renderArea.extent = engine->swapChainExtent;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, engine->graphicsPipeline);

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)(engine->swapChainExtent.width);
	viewport.height = (float)(engine->swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor = {};
	VkOffset2D scissorOffset = { 0, 0 };
	scissor.offset = scissorOffset;
	scissor.extent = engine->swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	// after vertex buffer memory
	VkBuffer vertexBuffers[] = { entity->vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	// vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
	vkCmdDraw(commandBuffer, entity->verticesCount, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		fputs("failed to record command buffer\n", stderr);
	}
}

void drawFrame(VulkanEngine *vulkan, VulkanEntity *entity) {
	// wait until previous frame has completely finished
	vkWaitForFences(
		vulkan->logicalDevice,
		1,
		&vulkan->inFlightFences[vulkan->currentFrame],
		VK_TRUE,
		UINT64_MAX);

	// clear the fences in preparation for drawing
	vkResetFences(
		vulkan->logicalDevice,
		1,
		&vulkan->inFlightFences[vulkan->currentFrame]);

	// get the next image from the swap chain and specify these semaphores
	// to be signaled when the presentation engine is finished with this image
	uint32_t imageIndex;
	vkAcquireNextImageKHR(
		vulkan->logicalDevice,
		vulkan->swapChain,
		UINT64_MAX,
		vulkan->imageAvailableSemaphores[vulkan->currentFrame],
		VK_NULL_HANDLE,
		&imageIndex);

	vkResetCommandBuffer(vulkan->commandBuffers[vulkan->currentFrame],  0);
	recordCommandBuffer(vulkan, entity, vulkan->commandBuffers[vulkan->currentFrame], imageIndex);

	// now we are done recording the command buffer, we can submit it
	VkSemaphore waitSemaphores[] = {
		vulkan->imageAvailableSemaphores[vulkan->currentFrame]
	};
	VkPipelineStageFlags waitStages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	};
	VkSemaphore signalSemaphores[] = {
		vulkan->renderFinishedSemaphores[vulkan->currentFrame]
	};
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vulkan->commandBuffers[vulkan->currentFrame];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VkResult resultQueueSubmit = vkQueueSubmit(
		vulkan->graphicsQueue,
		1,
		&submitInfo,
		vulkan->inFlightFences[vulkan->currentFrame]);
	if (resultQueueSubmit != VK_SUCCESS) {
		fputs("failed to submit draw command buffer\n", stderr);
	}

	// submit the result back into the swap chain so it can be presented on screen
	VkSwapchainKHR swapChains[] = { vulkan->swapChain };
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = NULL; // Optional

	vkQueuePresentKHR(vulkan->presentQueue, &presentInfo);

	vulkan->currentFrame = (vulkan->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
