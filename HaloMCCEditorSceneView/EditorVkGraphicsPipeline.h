#pragma once

#include "EditorVKCommon.h"
#include "VkShaderProgram.h"

#include <memory>

class EditorVKSwapchain;

class EditorVKGraphicsPipeline
{
public:
	EditorVKGraphicsPipeline() {}

	void CreateGraphicsPipeline(VkDevice logicalDevice, EditorVKSwapchain* swapchain, const std::string& shaderLocation);
	void CreateRenderPass(VkDevice logicalDevice, VkFormat framebufferFormat);

	void Cleanup(VkDevice logicalDevice);

public:
	VkRenderPass renderPass;
	VkPipeline graphicsPipeline;
	VkPipelineLayout graphicsPipelineLayout;

	std::unique_ptr<VkShaderProgram> defaultVertexShader;
	std::unique_ptr<VkShaderProgram> defaultFragmentShader;
};