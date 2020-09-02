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

	VkRenderPass GetRenderPassSetup() const { return renderPass; }
	VkPipeline GetGraphicsPipeline() const { return graphicsPipeline; }
	VkPipelineLayout GetGraphicsPipelineLayout() const { return graphicsPipelineLayout; }

	void Cleanup(VkDevice logicalDevice);

	void BeginFrame(VkCommandBuffer commandBuffer, VkFramebuffer targetFramebuffer, VkExtent2D renderExtent);
	void EndFrame(VkCommandBuffer commandBuffer);


public:
	VkRenderPass renderPass;
	VkPipeline graphicsPipeline;
	VkPipelineLayout graphicsPipelineLayout;

	std::unique_ptr<VkShaderProgram> defaultVertexShader;
	std::unique_ptr<VkShaderProgram> defaultFragmentShader;
};