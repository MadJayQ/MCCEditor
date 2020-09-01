#pragma once

#include "EditorVKCommon.h"
#include "VkShaderProgram.h"

#include <memory>

class EditorVKGraphicsPipeline
{
public:
	EditorVKGraphicsPipeline() {}

	void CreateGraphicsPipeline(VkDevice logicalDevice, const std::string& shaderLocation);

public:
	VkPipeline graphicsPipeline;

	std::unique_ptr<VkShaderProgram> defaultVertexShader;
	std::unique_ptr<VkShaderProgram> defaultFragmentShader;
};