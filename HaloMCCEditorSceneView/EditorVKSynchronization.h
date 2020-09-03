#pragma once

#include "EditorVKCommon.h"

class EditorVKSynchronization
{
public:
	EditorVKSynchronization() {}

	void CreateSemaphores(VkDevice logicalDevice);
	void Cleanup(VkDevice logicalDevice);

	uint32_t AquireSwapchainImage(VkDevice logicalDevice, VkSwapchainKHR swapchain);

	VkSemaphore GetImageAvailableSemaphore() const { return imageAvailableSemaphore; }
	VkSemaphore GetRenderFinishedSemaphore() const { return renderFinishedSemaphore; }

private:
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
};