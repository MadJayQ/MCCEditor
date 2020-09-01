#include "EditorVkGraphicsPipeline.h"


const std::string DEFAULT_VERT_SHADER_FILE = "vert.spv";
const std::string DEFAULT_FRAG_SHADER_FILE = "frag.spv";
const std::string DEFAULT_VERT_SHADER_NAME = "DEFAULT-VERTEX-SHADER";
const std::string DEFAULT_FRAG_SHADER_NAME = "DEFAULT-FRAGMENT-SHADER";


void EditorVKGraphicsPipeline::CreateGraphicsPipeline(VkDevice logicalDevice, const std::string& shaderLocation)
{
	defaultVertexShader = std::make_unique<VkShaderProgram>(DEFAULT_VERT_SHADER_NAME, ShaderType::VK_VERTEX_SHADER);
	defaultFragmentShader = std::make_unique<VkShaderProgram>(DEFAULT_FRAG_SHADER_NAME, ShaderType::VK_VERTEX_SHADER);


	std::string vertexPath = shaderLocation + "/" + DEFAULT_VERT_SHADER_FILE;
	std::string fragmentPath = shaderLocation + "/" + DEFAULT_FRAG_SHADER_FILE;

	defaultVertexShader->FromCompiledBinary(logicalDevice, vertexPath);
	defaultFragmentShader->FromCompiledBinary(logicalDevice, fragmentPath);

	VkPipelineShaderStageCreateInfo shaderStages[] =
	{
		defaultVertexShader->GenerateStageCreationInfo(),
		defaultFragmentShader->GenerateStageCreationInfo()
	};

	VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pStages = shaderStages;
}