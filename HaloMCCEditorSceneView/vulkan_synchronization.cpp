#include "EditorVKSynchronization.h"

#include <stdexcept>

void EditorVKSynchronization::CreateSynchronizationObjects(VkDevice logicalDevice, size_t numSwapchainImages)
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VkResult result = VK_SUCCESS;

	imageAvailableSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(numSwapchainImages, VK_NULL_HANDLE);


	for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore[i]);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image available semaphore!");
		}
		result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphore[i]);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create render finished semaphore!");
		}
		result = vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create GPU-CPU synchronization fence!");
		}
	}

}

VkFence EditorVKSynchronization::GetCPUSynchronizationFence(VkDevice logicalDevice) const
{
	vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);
	return inFlightFences[currentFrame];
}

void EditorVKSynchronization::SynchronizeFramebuffer(VkDevice logicalDevice, uint32_t framebufferIndex)
{
	//If a previous frame has used this image, make sure we're not going to write to it while the frame is in flight
	if (imagesInFlight[framebufferIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(logicalDevice, 1, &imagesInFlight[framebufferIndex], VK_TRUE, UINT64_MAX);
	}
	imagesInFlight[framebufferIndex] = inFlightFences[currentFrame];
}

void EditorVKSynchronization::WaitOnCPUSynchronization(VkDevice logicalDevice)
{
	vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
}

void EditorVKSynchronization::Cleanup(VkDevice logicalDevice)
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphore[i], nullptr);
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphore[i], nullptr);
		vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
	}
}

uint32_t EditorVKSynchronization::AquireSwapchainImage(VkDevice logicalDevice, VkSwapchainKHR swapchain)
{
	uint32_t nextAvailableIndex = 0UL;
	VkResult result = vkAcquireNextImageKHR(logicalDevice, swapchain, UINT64_MAX, imageAvailableSemaphore[currentFrame], VK_NULL_HANDLE, &nextAvailableIndex);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to aquire a swapchain image!");
	}

	return nextAvailableIndex;
}
