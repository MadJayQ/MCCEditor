#include "VkShaderProgram.h"

#include <fstream>


VkShaderStageFlagBits utils_DetermineShaderStageBitFlags(ShaderType type)
{
	switch (type)
	{
	case ShaderType::VK_VERTEX_SHADER: return VK_SHADER_STAGE_VERTEX_BIT;
	case ShaderType::VK_FRAGMENT_SHADER: return VK_SHADER_STAGE_FRAGMENT_BIT;
	default: VK_SHADER_STAGE_ALL_GRAPHICS;
	}
}

void VkShaderProgram::FromCompiledBinary(VkDevice logicalDevice, const std::string& binaryPath)
{
	std::vector<char> shaderByteCode = read_shader_from_disk(binaryPath);
	compile_shader_module(logicalDevice, shaderByteCode);
}

void VkShaderProgram::Cleanup(VkDevice logicalDevice)
{
	vkDestroyShaderModule(logicalDevice, shaderModule, nullptr);
}

VkPipelineShaderStageCreateInfo VkShaderProgram::GenerateStageCreationInfo()
{
	VkPipelineShaderStageCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.stage = utils_DetermineShaderStageBitFlags(shaderType);
	createInfo.module = shaderModule;
	createInfo.pName = shaderName.c_str();

	return createInfo;
}

std::vector<char> VkShaderProgram::read_shader_from_disk(const std::string& fileName)
{
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open shader file: " + fileName);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> fileBuffer(fileSize);

	file.seekg(0);
	file.read(fileBuffer.data(), fileSize);
	file.close();

	return fileBuffer;
}

void VkShaderProgram::compile_shader_module(VkDevice logicalDevice, const std::vector<char>& codeBuffer)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = codeBuffer.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(codeBuffer.data());

	VkResult result = vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module!");
	}
}

