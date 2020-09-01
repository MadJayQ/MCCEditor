#pragma once

#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include <vector>

#include <optional>

struct QueueFamilyIndices;
struct SwapChainSupportDetails;

class EditorVKContext {
public:
	explicit EditorVKContext(HWND hwnd, RECT rect);

	void InitializeVulkan();
	void Cleanup();

private:
	void create_vulkan_instance();
	void create_vulkan_surface();

	void create_debug_messenger();

	void select_physical_device();
	void create_logical_device();
	void create_device_swapchain();
	void create_image_views();

	VkSurfaceFormatKHR select_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR select_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D select_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);


	bool check_validation_layers();
	bool check_valid_device(VkPhysicalDevice device);

	QueueFamilyIndices find_queue_families(VkPhysicalDevice device);
	SwapChainSupportDetails query_swapchain_support(VkPhysicalDevice device);

	void populate_debug_messenger_struct(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

private:
	VkInstance vulkanInstance;
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;

	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;

	VkDebugUtilsMessengerEXT debugMessenger;

	HWND windowHandle;
	RECT clientRect;
};

extern std::shared_ptr<EditorVKContext> vkCtx;