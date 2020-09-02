#pragma once

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include <vector>

#include <optional>

#include "EditorVKCommon.h"
#include "EditorVKSwapchain.h"
#include "EditorVkGraphicsPipeline.h"

class EditorVKContext 
{
public:
	explicit EditorVKContext(HWND hwnd, RECT rect);

	void InitializeVulkan();
	void InitializeSwapchain();
	void InitializeGraphicsPipeline(const std::string& shaderLocation);
	void Cleanup();

	void BeginFrame();
	void EndFrame();

private:
	void create_vulkan_instance();
	void create_vulkan_surface();

	void create_debug_messenger();

	void select_physical_device();
	void create_logical_device();

	void create_command_pool();

	bool check_validation_layers();
	bool check_valid_device(VkPhysicalDevice device);

	void populate_debug_messenger_struct(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

private:
	VkInstance vulkanInstance;
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;

	VkDebugUtilsMessengerEXT debugMessenger;

	HWND windowHandle;
	RECT clientRect;

	std::unique_ptr<EditorVKSwapchain> swapChainPtr;
	std::unique_ptr<EditorVKGraphicsPipeline> graphicsPipeline;
};

extern std::shared_ptr<EditorVKContext> vkCtx;