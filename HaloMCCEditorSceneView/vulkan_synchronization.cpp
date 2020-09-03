#include "EditorVKSynchronization.h"

#include <stdexcept>

void EditorVKSynchronization::CreateSemaphores(VkDevice logicalDevice)
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkResult result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create semaphores!");
	}

	result = vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphore);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create render finished semaphore!");
	}

}

void EditorVKSynchronization::Cleanup(VkDevice logicalDevice)
{
	vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
}

uint32_t EditorVKSynchronization::AquireSwapchainImage(VkDevice logicalDevice, VkSwapchainKHR swapchain)
{
	uint32_t nextAvailableIndex = 0UL;
	VkResult result = vkAcquireNextImageKHR(logicalDevice, swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &nextAvailableIndex);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to aquire a swapchain image!");
	}

	return nextAvailableIndex;
}
