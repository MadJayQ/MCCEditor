#pragma once

#include "EditorVKCommon.h"

#include <string>

enum class ShaderType { VK_VERTEX_SHADER, VK_FRAGMENT_SHADER };

class VkShaderProgram
{
public:
	explicit VkShaderProgram(const std::string& name, ShaderType _shaderType) 
		: shaderName(name), shaderType(_shaderType) {}

	void FromCompiledBinary(VkDevice logicalDevice, const std::string& binaryPath);
	void Cleanup(VkDevice logicalDevice);

	VkPipelineShaderStageCreateInfo GenerateStageCreationInfo();

private:
	std::vector<char> read_shader_from_disk(const std::string& fileName);
	void compile_shader_module(VkDevice logicalDevice, const std::vector<char>& codebuffer);

private:
	std::string shaderName;
	ShaderType shaderType;
	VkShaderModule shaderModule;
};