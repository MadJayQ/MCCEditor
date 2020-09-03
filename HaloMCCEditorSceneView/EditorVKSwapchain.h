#pragma once

#include "EditorVKCommon.h"


class EditorVKGraphicsPipeline;
class EditorVKSynchronization;

class EditorVKSwapchain
{
public:
	explicit EditorVKSwapchain() {}

	void CreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface);
	void Cleanup(VkPhysicalDevice physicalDevice, VkDevice logicalDevice);

	bool DeviceHasValidSwapchain(VkPhysicalDevice device, VkSurfaceKHR surface);

	void SetClientRect(RECT rect) { clientRect = rect; }
	RECT GetClientRect() const { return clientRect; }

	VkExtent2D Extent() const { return swapChainExtent; }
	VkFormat FramebufferFromat() const { return swapChainImageFormat; }

	void CreateFramebuffers(VkDevice device, EditorVKGraphicsPipeline* pipeline);
	VkFramebuffer GetFramebuffer(int index);
	int GetFramebufferCount() const { return swapChainFramebuffers.size(); }
	

	uint32_t AquireImage(VkDevice logicalDevice, EditorVKSynchronization* synchronizer);

	void Present(VkQueue presentQueue, EditorVKSynchronization* synchronizer, uint32_t imageIdx);
	bool Valid() const { return swapchainInvalid; }

private:

	void create_image_views(VkDevice logicalDevice);

	VkSurfaceFormatKHR select_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR select_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D select_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);


private:

	VkSwapchainKHR swapChain;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;

	std::vector<VkFramebuffer> swapChainFramebuffers;

	bool swapchainInvalid;

	RECT clientRect;


};