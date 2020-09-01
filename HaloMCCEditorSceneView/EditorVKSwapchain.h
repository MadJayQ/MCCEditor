#pragma once

#include "EditorVKCommon.h"



class EditorVKSwapchain
{
public:
	explicit EditorVKSwapchain() {}

	void CreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface);
	void Cleanup(VkPhysicalDevice physicalDevice, VkDevice logicalDevice);

	bool DeviceHasValidSwapchain(VkPhysicalDevice device, VkSurfaceKHR surface);

	void SetClientRect(RECT rect) { clientRect = rect; }
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

	RECT clientRect;


};