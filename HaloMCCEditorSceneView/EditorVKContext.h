#pragma once

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include <vector>

#include <optional>

#include "EditorVKCommon.h"
#include "EditorVKSwapchain.h"
#include "EditorVkGraphicsPipeline.h"
#include "EditorVKSynchronization.h"

class EditorVKContext 
{
public:
	explicit EditorVKContext(HWND hwnd, RECT rect);

	void InitializeVulkan();
	void InitializeSwapchain();
	void InitializeGraphicsPipeline();
	void Cleanup();

	void BeginFrame();
	void EndFrame();
	void SubmitAndFlip();

	void OnWindowResize(RECT newClientSize);
	void RecreateResources();

	void SetShaderDirectory(const std::string& directory);

private:
	void create_vulkan_instance();
	void create_vulkan_surface();

	void create_debug_messenger();

	void select_physical_device();
	void create_logical_device();

	void create_command_pool();
	void create_synchronization_objects();

	bool check_validation_layers();
	bool check_valid_device(VkPhysicalDevice device);

	void destroy_command_pool();

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
	std::unique_ptr<EditorVKSynchronization> synchronization;

	bool contextValid;
};

extern std::shared_ptr<EditorVKContext> vkCtx;