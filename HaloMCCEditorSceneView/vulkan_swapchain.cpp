#include "EditorVKSwapchain.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <stdexcept>

#include <algorithm>

#include "EditorVkGraphicsPipeline.h"
#include "EditorVKSynchronization.h"
#include "EditorVKUtils.hpp"


void EditorVKSwapchain::CreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface)
{
	SwapChainSupportDetails swapChainSupport = utils_QuerySwapChainSupport(physicalDevice, surface);

	VkSurfaceFormatKHR surfaceFormat = select_swap_surface_format(swapChainSupport.formats);
	VkPresentModeKHR presentMode = select_swap_present_mode(swapChainSupport.presentModes);
	VkExtent2D swapExtents = select_swap_extent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = swapExtents;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = utils_QueryQueueFamilies(physicalDevice, surface);

	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = swapExtents;

	create_image_views(logicalDevice);
}

void EditorVKSwapchain::Cleanup(VkPhysicalDevice device, VkDevice logicalDevice)
{
	for (VkFramebuffer framebuffer : swapChainFramebuffers)
	{
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
	}
	for (VkImageView imageView : swapChainImageViews)
	{
		vkDestroyImageView(logicalDevice, imageView, nullptr);
	}
	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
}

bool EditorVKSwapchain::DeviceHasValidSwapchain(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	SwapChainSupportDetails swapChainSupport = utils_QuerySwapChainSupport(device, surface);
	
	return !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
}

void EditorVKSwapchain::CreateFramebuffers(VkDevice device, EditorVKGraphicsPipeline* pipeline)
{
	swapChainFramebuffers.resize(swapChainImageViews.size());
	for (size_t i = 0; i < swapChainImageViews.size(); i++)
	{
		VkImageView attachments[] = 
		{
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = pipeline->GetRenderPassSetup();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		VkResult result = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create framebuffer!");
		}
	}
}

VkFramebuffer EditorVKSwapchain::GetFramebuffer(int index)
{
	if (index >= swapChainFramebuffers.size())
	{
		throw std::runtime_error("Attempting to index an invalid framebuffer");
	}

	return swapChainFramebuffers[index];
}

uint32_t EditorVKSwapchain::AquireImage(VkDevice logicalDevice, EditorVKSynchronization* synchronizer)
{
	uint32_t nextImageIndex = synchronizer->AquireSwapchainImage(logicalDevice, swapChain);
	if (nextImageIndex == -1 || nextImageIndex >= swapChainImages.size())
	{
		throw std::runtime_error("Attempted to aquire an invalid image from the swap chain!");
	}

	return nextImageIndex;
}

void EditorVKSwapchain::Present(VkQueue presentQueue, EditorVKSynchronization* synchronizer, uint32_t imageIdx)
{
	VkSemaphore signalSemaphore = synchronizer->GetRenderFinishedSemaphore();

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &signalSemaphore;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIdx;
	presentInfo.pResults = nullptr; // Optional

	VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		swapchainInvalid = true;
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swapchain!");
	}
}

void EditorVKSwapchain::create_image_views(VkDevice logicalDevice)
{
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkResult result = vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image views!");
		}
	}
}

VkSurfaceFormatKHR EditorVKSwapchain::select_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	VkFormat targetFormat = VK_FORMAT_B8G8R8A8_SRGB;
	VkColorSpaceKHR targetColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	for (const VkSurfaceFormatKHR& availableFormat : availableFormats)
	{
		if (availableFormat.format == targetFormat && availableFormat.colorSpace == targetColorSpace)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR EditorVKSwapchain::select_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	VkPresentModeKHR targetPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

	for (const VkPresentModeKHR& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == targetPresentMode)
		{
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D EditorVKSwapchain::select_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = { clientRect.right, clientRect.bottom };

		actualExtent.width = std::max<uint32_t>(capabilities.minImageExtent.width, std::min<uint32_t>(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max<uint32_t>(capabilities.minImageExtent.height, std::min<uint32_t>(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}
