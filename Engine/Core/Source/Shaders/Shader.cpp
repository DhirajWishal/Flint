// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Shaders/Shader.hpp"
#include "Core/Errors/AssetError.hpp"

#include <spirv_reflect.h>
#include <spdlog/spdlog.h>

#include <fstream>

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
	 * Get the resource type from the reflection type.
	 *
	 * @param type The reflection type.
	 * @return The resource type.
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
		default:																spdlog::error("Invalid resource type!"); return Flint::ResourceType::Undefined;
		}
	}
}


namespace Flint
{
	Shader::Shader(std::filesystem::path&& file)
	{
		std::fstream shaderFile(file, std::ios::in | std::ios::ate | std::ios::binary);

		// Check if we opened the file.
		if (!shaderFile.is_open())
			throw AssetError("Failed to load the shader file!");

		// Load the shader data.
		const auto fileSize = shaderFile.tellg();
		shaderFile.seekg(0);

		// Resize the buffer.
		m_Code.resize(fileSize);

		// Now let's load it.
		shaderFile.read(reinterpret_cast<char*>(m_Code.data()), fileSize);
		shaderFile.close();

		// Perform reflection over the shader code.
		reflect();
	}

	void Shader::setEntryPoint(std::string&& name) noexcept
	{
		m_EntryPoint = std::move(name);
	}

	void Shader::reflect()
	{
		SpvReflectShaderModule shaderModule = {};
		const auto shaderCode = std::vector<uint32_t>(m_Code.begin(), m_Code.begin() + (m_Code.size() / 4));
		ValidateReflection(spvReflectCreateShaderModule(shaderCode.size() * sizeof(uint32_t), shaderCode.data(), &shaderModule));

		// Resolve uniforms.
		{
			uint32_t variableCount = 0;
			ValidateReflection(spvReflectEnumerateDescriptorBindings(&shaderModule, &variableCount, nullptr));

			std::vector<SpvReflectDescriptorBinding*> pBindings(variableCount);
			ValidateReflection(spvReflectEnumerateDescriptorBindings(&shaderModule, &variableCount, pBindings.data()));

			// Iterate over the resources and setup the bindings.
			m_Bindings.reserve(pBindings.size());
			for (const auto& pResource : pBindings)
			{
				auto& binding = m_Bindings.emplace_back();
				binding.m_Set = pResource->set;
				binding.m_Binding = pResource->binding;
				binding.m_Count = pResource->count;
				binding.m_Type = GetResourceType(pResource->descriptor_type);
			}
		}

		// Resolve push constants.
		{
			uint32_t variableCount = 0;
			ValidateReflection(spvReflectEnumeratePushConstantBlocks(&shaderModule, &variableCount, nullptr));

			std::vector<SpvReflectBlockVariable*> pPushConstants(variableCount);
			ValidateReflection(spvReflectEnumeratePushConstantBlocks(&shaderModule, &variableCount, pPushConstants.data()));

			// Iterate over the push constants and setup.
			m_PushConstants.reserve(pPushConstants.size());
			for (const auto& resource : pPushConstants)
			{
				auto& pushConstant = m_PushConstants.emplace_back();
				pushConstant.m_Size = resource->size;
				pushConstant.m_Offset = resource->offset;
			}
		}
	}
}