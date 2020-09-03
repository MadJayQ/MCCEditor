#pragma once

#include "EditorVKCommon.h"

static constexpr const int MAX_FRAMES_IN_FLIGHT = 2;

class EditorVKSynchronization
{
public:
	EditorVKSynchronization() {}

	void CreateSynchronizationObjects(VkDevice logicalDevice, size_t numSwapchainImages);
	void Cleanup(VkDevice logicalDevice);

	uint32_t AquireSwapchainImage(VkDevice logicalDevice, VkSwapchainKHR swapchain);

	VkSemaphore GetImageAvailableSemaphore() const { return imageAvailableSemaphore[currentFrame]; }
	VkSemaphore GetRenderFinishedSemaphore() const { return renderFinishedSemaphore[currentFrame]; }
	VkFence GetCPUSynchronizationFence(VkDevice logicalDevice) const;

	void SynchronizeFramebuffer(VkDevice logicalDevice, uint32_t framebufferIndex);
	void WaitOnCPUSynchronization(VkDevice logicalDevice);

	void IncrementCPUFrame() { currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT; }
	uint32_t CurrentCPUFrame() const { return currentFrame; }

private:
	std::vector<VkSemaphore> imageAvailableSemaphore;
	std::vector<VkSemaphore> renderFinishedSemaphore;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	uint32_t currentFrame;
};