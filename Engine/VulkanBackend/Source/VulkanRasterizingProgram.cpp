// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanRasterizingProgram.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

#include <Optick.h>
#include <spirv_reflect.h>
#include <spdlog/spdlog.h>

namespace /* anonymous */
{
	/**
	 * Validate the reflection result.
	 *
	 * @param result The reflection result.
	 */
	void ValidateReflection(const SpvReflectResult result)
	{
		switch (result)
		{
		case SPV_REFLECT_RESULT_SUCCESS:										return;
		case SPV_REFLECT_RESULT_NOT_READY:										spdlog::error("Shader not ready!"); break;
		case SPV_REFLECT_RESULT_ERROR_PARSE_FAILED:								spdlog::error("Shader parse failed!"); break;
		case SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED:								spdlog::error("Shader allocation failed!"); break;
		case SPV_REFLECT_RESULT_ERROR_RANGE_EXCEEDED:							spdlog::error("Shader range exceeded!"); break;
		case SPV_REFLECT_RESULT_ERROR_NULL_POINTER:								spdlog::error("Shader null pointer!"); break;
		case SPV_REFLECT_RESULT_ERROR_INTERNAL_ERROR:							spdlog::error("Shader internal reflection error!"); break;
		case SPV_REFLECT_RESULT_ERROR_COUNT_MISMATCH:							spdlog::error("Shader count mismatch!"); break;
		case SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND:						spdlog::error("Shader element not found!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_CODE_SIZE:					spdlog::error("Shader invalid SPIRV code size!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_MAGIC_NUMBER:				spdlog::error("Shader invalid SPIRV magic number!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_EOF:						spdlog::error("Shader SPIRV unexpected end of file (EOF)!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ID_REFERENCE:				spdlog::error("Shader invalid SPIRV ID reference!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_SET_NUMBER_OVERFLOW:				spdlog::error("Shader invalid SPIRV descriptor set number overflow!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_STORAGE_CLASS:				spdlog::error("Shader invalid SPIRV storage class!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_RECURSION:							spdlog::error("Shader invalid SPIRV recursion!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_INSTRUCTION:				spdlog::error("Shader invalid SPIRV instruction!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_BLOCK_DATA:				spdlog::error("Shader invalid SPIRV block data!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_BLOCK_MEMBER_REFERENCE:		spdlog::error("Shader invalid SPIRV block member reference!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ENTRY_POINT:				spdlog::error("Shader invalid SPIRV entry point!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_EXECUTION_MODE:				spdlog::error("Shader invalid SPIRV execution mode!"); break;
		default:																spdlog::error("Unknown reflection error!");
		}
	}

	/**
	 * Get the descriptor type from the reflection type.
	 *
	 * @param type The reflection type.
	 * @return The descriptor type.
	 */
	VkDescriptorType GetDescriptorType(SpvReflectDescriptorType type)
	{
		switch (type)
		{
		case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:								return VK_DESCRIPTOR_TYPE_SAMPLER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:				return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:							return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:							return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:					return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:					return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:						return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:						return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:				return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:				return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:						return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:			return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		default:																spdlog::error("Invalid resource type!"); return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		}
	}

	/**
	 * Get the binging type from the reflection type.
	 *
	 * @parma type The reflection type.
	 * @return The binding type.
	 */
	Flint::ResourceType GetResourceType(SpvReflectDescriptorType type)
	{
		switch (type)
		{
		case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:								return Flint::ResourceType::Sampler;
		case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:				return Flint::ResourceType::CombinedImageSampler;
		case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:							return Flint::ResourceType::SampledImage;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:							return Flint::ResourceType::StorageImage;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:					return Flint::ResourceType::UniformTexelBuffer;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:					return Flint::ResourceType::StorageTexelBuffer;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:						return Flint::ResourceType::UniformBuffer;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:						return Flint::ResourceType::StorageBuffer;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:				return Flint::ResourceType::DynamicUniformBuffer;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:				return Flint::ResourceType::DynamicStorageBuffer;
		case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:						return Flint::ResourceType::InputAttachment;
		case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:			return Flint::ResourceType::AccelerationStructure;
		default:																spdlog::error("Invalid resource type!"); return Flint::ResourceType::UniformBuffer;
		}
	}

	/**
	 * Get the Vulkan format from the reflection format.
	 *
	 * @param format The reflection format.
	 * @return The Vulkan format.
	 */
	VkFormat GetFormat(SpvReflectFormat format)
	{
		// Thankfully the SPIRV-Reflect format is the same as the Vulkan format so we can simply static cast it.
		return static_cast<VkFormat>(format);
	}
}

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanRasterizingProgram::VulkanRasterizingProgram(const std::shared_ptr<VulkanDevice>& pDevice, ShaderCode&& vertexShader, ShaderCode&& fragmetShader)
			: RasterizingProgram(pDevice, std::move(vertexShader), std::move(fragmetShader))
		{
			OPTICK_EVENT();

			std::vector<VkPushConstantRange> pushConstants;

			// Create the shader modules.
			if (!m_VertexShader.empty())
				m_VertexShaderModule = createShaderModule(m_VertexShader, VK_SHADER_STAGE_VERTEX_BIT, pushConstants);

			if (!m_FragmentShader.empty())
				m_FragmentShaderModule = createShaderModule(m_FragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT, pushConstants);

			// Create the descriptor set layout and the pipeline layout.
			createDescriptorSetLayout();
			createPipelineLayout(std::move(pushConstants));

			// Make sure to set the object as valid.
			validate();
		}

		VulkanRasterizingProgram::~VulkanRasterizingProgram()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanRasterizingProgram::terminate()
		{
			OPTICK_EVENT();

			if (m_VertexShaderModule)
				getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyShaderModule(getDevice().as<VulkanDevice>()->getLogicalDevice(), m_VertexShaderModule, nullptr);

			if (m_FragmentShaderModule)
				getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyShaderModule(getDevice().as<VulkanDevice>()->getLogicalDevice(), m_FragmentShaderModule, nullptr);

			getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyDescriptorSetLayout(getDevice().as<VulkanDevice>()->getLogicalDevice(), m_DescriptorSetLayout, nullptr);
			getDevice().as<VulkanDevice>()->getDeviceTable().vkDestroyPipelineLayout(getDevice().as<VulkanDevice>()->getLogicalDevice(), m_PipelineLayout, nullptr);

			invalidate();
		}

		VkShaderModule VulkanRasterizingProgram::createShaderModule(const ShaderCode& shader, VkShaderStageFlags stageFlags, std::vector<VkPushConstantRange>& pushConstants)
		{
			OPTICK_EVENT();

			const auto& shaderCode = shader.get();

			// Prepare the shader code for reflection.
			const auto reflectionSource = std::vector<uint32_t>(shaderCode.begin(), shaderCode.begin() + (shaderCode.size() / 4));

			SpvReflectShaderModule reflectionModule = {};
			ValidateReflection(spvReflectCreateShaderModule(reflectionSource.size() * sizeof(uint32_t), reflectionSource.data(), &reflectionModule));

			// Resolve shader inputs.
			if (stageFlags & VK_SHADER_STAGE_VERTEX_BIT)
			{
				uint32_t variableCount = 0;
				ValidateReflection(spvReflectEnumerateInputVariables(&reflectionModule, &variableCount, nullptr));

				std::vector<SpvReflectInterfaceVariable*> pInputs(variableCount);
				ValidateReflection(spvReflectEnumerateInputVariables(&reflectionModule, &variableCount, pInputs.data()));

				// Iterate through the attributes and load them.
				for (auto& pResource : pInputs)
				{
					if (pResource->format == SpvReflectFormat::SPV_REFLECT_FORMAT_UNDEFINED || pResource->built_in != -1)
						continue;

					// Store the input as a vertex input if the location is in the vertex attribute range. 
					if (pResource->location < EnumToInt(VertexAttribute::Max))
					{
						auto& input = m_VertexInputs.emplace_back();
						input.m_Components = std::max(pResource->type_description->traits.numeric.vector.component_count, 1u);
						input.m_Attribute = static_cast<VertexAttribute>(pResource->location);
					}

					// If not and if the location is within the instance attribute range, it's considered to be an instance input.
					else if (pResource->location < EnumToInt(InstanceAttribute::Max))
					{
						auto& input = m_InstanceInputs.emplace_back();
						input.m_Components = std::max(pResource->type_description->traits.numeric.vector.component_count, 1u);
						input.m_Attribute = static_cast<InstanceAttribute>(pResource->location);
					}

					//  Else we throw an error.
					else
						throw BackendError("Invalid Vertex shader input found!");
				}
			}

			// Load all the layout bindings.
			{
				uint32_t variableCount = 0;
				ValidateReflection(spvReflectEnumerateDescriptorBindings(&reflectionModule, &variableCount, nullptr));

				std::vector<SpvReflectDescriptorBinding*> pBindings(variableCount);
				ValidateReflection(spvReflectEnumerateDescriptorBindings(&reflectionModule, &variableCount, pBindings.data()));

				// Iterate over the resources and setup the bindings.
				for (const auto& pResource : pBindings)
				{
					auto& binding = m_LayoutBindings.emplace_back();
					binding.binding = pResource->binding;
					binding.descriptorCount = pResource->count;
					binding.descriptorType = GetDescriptorType(pResource->descriptor_type);
					binding.pImmutableSamplers = nullptr;
					binding.stageFlags = stageFlags;

					auto& poolSize = m_PoolSizes.emplace_back();
					poolSize.descriptorCount = pResource->count;
					poolSize.type = binding.descriptorType;

					m_BindingMap.registerBinding(pResource->name, pResource->binding, GetResourceType(pResource->descriptor_type));
				}
			}

			// Resolve push constants.
			{
				uint32_t variableCount = 0;
				ValidateReflection(spvReflectEnumeratePushConstantBlocks(&reflectionModule, &variableCount, nullptr));

				std::vector<SpvReflectBlockVariable*> pPushConstants(variableCount);
				ValidateReflection(spvReflectEnumeratePushConstantBlocks(&reflectionModule, &variableCount, pPushConstants.data()));

				// Iterate over the push constants and setup.
				for (const auto& resource : pPushConstants)
				{
					auto& pushConstant = pushConstants.emplace_back();
					pushConstant.size = resource->size;
					pushConstant.offset = resource->offset;
					pushConstant.stageFlags = stageFlags;
				}
			}

			// Now let's create the shader module.
			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.flags = 0;
			createInfo.pNext = nullptr;
			createInfo.codeSize = shader.getSize();
			createInfo.pCode = shaderCode.data();

			VkShaderModule shaderModule = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkCreateShaderModule(getDevice().as<VulkanDevice>()->getLogicalDevice(), &createInfo, nullptr, &shaderModule), "Failed to create the shader module!");

			auto& shaderStage = m_ShaderStageCreateInfos.emplace_back();
			shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStage.flags = 0;
			shaderStage.pNext = nullptr;
			shaderStage.module = shaderModule;
			shaderStage.pSpecializationInfo = nullptr;
			shaderStage.stage = static_cast<VkShaderStageFlagBits>(stageFlags);
			shaderStage.pName = "main";

			return shaderModule;
		}

		void VulkanRasterizingProgram::createDescriptorSetLayout()
		{
			OPTICK_EVENT();

			VkDescriptorSetLayoutCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			createInfo.flags = 0;
			createInfo.pNext = nullptr;
			createInfo.bindingCount = static_cast<uint32_t>(m_LayoutBindings.size());
			createInfo.pBindings = m_LayoutBindings.data();

			FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkCreateDescriptorSetLayout(getDevice().as<VulkanDevice>()->getLogicalDevice(), &createInfo, nullptr, &m_DescriptorSetLayout), "Failed to create the descriptor set layout!");
		}

		void VulkanRasterizingProgram::createPipelineLayout(std::vector<VkPushConstantRange>&& pushConstants)
		{
			OPTICK_EVENT();

			VkPipelineLayoutCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			createInfo.flags = 0;
			createInfo.pNext = nullptr;
			createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
			createInfo.pPushConstantRanges = pushConstants.data();
			createInfo.setLayoutCount = 1;
			createInfo.pSetLayouts = &m_DescriptorSetLayout;

			FLINT_VK_ASSERT(getDevice().as<VulkanDevice>()->getDeviceTable().vkCreatePipelineLayout(getDevice().as<VulkanDevice>()->getLogicalDevice(), &createInfo, nullptr, &m_PipelineLayout), "Failed to create the pipeline layout!");
		}
	}
}