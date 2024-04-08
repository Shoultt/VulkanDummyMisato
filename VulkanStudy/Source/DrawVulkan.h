#include "Source/initVulkan.h"
#include "Control.h"

void updateMVP()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	
	glm::mat4 model = glm::translate(glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f)), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(0.0f, 0.0, -10.0f));
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glm::mat4 projection = glm::perspective(glm::radians(fov), static_cast<float>(Wwidth) / static_cast<float>(Wheight), 0.1f, 1000.0f);
	projection[1][1] *= -1;

	uniformBufferObject1 ubo1{
	.model = model,
	.view = view,
	.proj = projection,
	.lightPos = glm::vec3(0, 3, 1) };

	memcpy(uniformBufferAllocInfo.pMappedData, &ubo1, sizeof(ubo1));
}

void recordDrawCmdBuffer(uint32_t imageIndex)
{
	const VkCommandBufferBeginInfo beginInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	vkBeginCommandBuffer(cmdBuffer, &beginInfo);
	{
		VkRenderPassBeginInfo rpbi = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		rpbi.renderPass = renPass;
		rpbi.framebuffer = frameBuffers[imageIndex];
		rpbi.renderArea.offset = { 0, 0 };
		rpbi.renderArea.extent = { Wwidth, Wheight };
		VkClearValue colorValue;
		if (hiddenCursor == 0)
			colorValue = { 0.2f, 0.2f, 0.2f, 1.0f };
		else
			colorValue = { 1.0f, 0.0f, 0.0f, 1.0f };
		VkClearValue depthValue = { 1.0f, 0.0f };
		std::vector<VkClearValue> clearValues = { colorValue, depthValue };
		rpbi.clearValueCount = clearValues.size();
		rpbi.pClearValues = clearValues.data();
		vkCmdBeginRenderPass(cmdBuffer, &rpbi, VK_SUBPASS_CONTENTS_INLINE);
		{
			vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeLayout, 0, 1, &descriptorSet, 0, nullptr);
			vkCmdBindIndexBuffer(cmdBuffer, indexBuffer1, 0, VK_INDEX_TYPE_UINT32);
			vkCmdBindVertexBuffers(cmdBuffer, 1, 1, &vertexBuffer1, offsets);
			vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphPipeline1);
			vkCmdDrawIndexed(cmdBuffer, monster.getIndicesCount(), 1, 0, 0, 0);
			vkCmdBindIndexBuffer(cmdBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
			vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffer, offsets);
			vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphPipeline0);
			vkCmdDrawIndexed(cmdBuffer, chaika.getIndicesCount(), 1, 0, 0, 0);
		}
		vkCmdEndRenderPass(cmdBuffer);
	}
	vkEndCommandBuffer(cmdBuffer);
}
void presentFrame()
{
	vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
	vkResetFences(device, 1, &fence);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, waitSemaphore, VK_NULL_HANDLE, &imageIndex);

	vkResetCommandBuffer(cmdBuffer, 0);
	recordDrawCmdBuffer(imageIndex);

	VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	const VkSubmitInfo submitInfo{ .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
	.waitSemaphoreCount = 1,
	.pWaitSemaphores = &waitSemaphore,
	.pWaitDstStageMask = &dstStageMask,
	.commandBufferCount = 1,
	.pCommandBuffers = &cmdBuffer,
	.signalSemaphoreCount = 1,
	.pSignalSemaphores = &signalSemaphore,
	};
	vkQueueSubmit(queue, 1, &submitInfo, fence);

	const VkPresentInfoKHR presentInfo{ .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
	.waitSemaphoreCount = 1,
	.pWaitSemaphores = &signalSemaphore,
	.swapchainCount = 1,
	.pSwapchains = &swapchain,
	.pImageIndices = &imageIndex };
	vkQueuePresentKHR(queue, &presentInfo);
}

void loop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		control();
		updateMVP();
		presentFrame();
	}
}