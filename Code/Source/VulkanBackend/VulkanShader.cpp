// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanShader.hpp"
#include "VulkanBackend/VulkanUtilities.hpp"

#include "spirv_reflect.h"
#include <fstream>

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{
			std::vector<UI32> ResolvePadding(const std::vector<UI32>& code)
			{
				const UI64 finalCodeSize = code.size() / 4;
				std::vector<UI32> resolvedCode(finalCodeSize);
				std::copy(code.begin(), code.begin() + finalCodeSize, resolvedCode.data());

				return resolvedCode;
			}

			void ValidateReflection(SpvReflectResult result)
			{
				switch (result)
				{
				case SPV_REFLECT_RESULT_ERROR_PARSE_FAILED:
					FLINT_THROW_BACKEND_ERROR("Shader parse failed!");

				case SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED:
					FLINT_THROW_BACKEND_ERROR("Shader allocation failed!");

				case SPV_REFLECT_RESULT_ERROR_RANGE_EXCEEDED:
					FLINT_THROW_BACKEND_ERROR("Shader range exceeded!");

				case SPV_REFLECT_RESULT_ERROR_NULL_POINTER:
					FLINT_THROW_BACKEND_ERROR("Shader null pointer!");

				case SPV_REFLECT_RESULT_ERROR_INTERNAL_ERROR:
					FLINT_THROW_BACKEND_ERROR("Shader internal reflection error!");

				case SPV_REFLECT_RESULT_ERROR_COUNT_MISMATCH:
					FLINT_THROW_BACKEND_ERROR("Shader count mismatch!");

				case SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND:
					FLINT_THROW_BACKEND_ERROR("Shader element not found!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_CODE_SIZE:
					FLINT_THROW_BACKEND_ERROR("Shader invalid SPIRV code size!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_MAGIC_NUMBER:
					FLINT_THROW_BACKEND_ERROR("Shader invalid SPIRV magic number!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_EOF:
					FLINT_THROW_BACKEND_ERROR("Shader SPIRV unexpected end of file (EOF)!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ID_REFERENCE:
					FLINT_THROW_BACKEND_ERROR("Shader invalid SPIRV ID reference!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_SET_NUMBER_OVERFLOW:
					FLINT_THROW_BACKEND_ERROR("Shader invalid SPIRV descriptor set number overflow!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_STORAGE_CLASS:
					FLINT_THROW_BACKEND_ERROR("Shader invalid SPIRV storage class!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_RECURSION:
					FLINT_THROW_BACKEND_ERROR("Shader invalid SPIRV recursion!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_INSTRUCTION:
					FLINT_THROW_BACKEND_ERROR("Shader invalid SPIRV instruction!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_BLOCK_DATA:
					FLINT_THROW_BACKEND_ERROR("Shader invalid SPIRV block data!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_BLOCK_MEMBER_REFERENCE:
					FLINT_THROW_BACKEND_ERROR("Shader invalid SPIRV block member reference!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ENTRY_POINT:
					FLINT_THROW_BACKEND_ERROR("Shader invalid SPIRV entry point!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_EXECUTION_MODE:
					FLINT_THROW_BACKEND_ERROR("Shader invalid SPIRV execution mode!");
				}
			}

			ShaderResourceType GetShaderResourceType(SpvReflectDescriptorType type)
			{
				switch (type)
				{
				case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
					return ShaderResourceType::SAMPLER;

				case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
					return ShaderResourceType::COMBINED_IMAGE_SAMPLER;

				case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
					return ShaderResourceType::SAMPLED_IMAGE;

				case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
					return ShaderResourceType::STORAGE_IMAGE;

				case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
					return ShaderResourceType::UNIFORM_TEXEL_BUFFER;

				case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
					return ShaderResourceType::STORAGE_TEXEL_BUFFER;

				case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
					return ShaderResourceType::UNIFORM_BUFFER;

				case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
					return ShaderResourceType::STORAGE_BUFFER;

				case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
					return ShaderResourceType::UNIFORM_BUFFER_DYNAMIC;

				case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
					return ShaderResourceType::STORAGE_BUFFER_DYNAMIC;

				case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
					return ShaderResourceType::INPUT_ATTACHMENT;

				case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
					return ShaderResourceType::ACCELERATION_STRUCTURE;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid shader descriptor type!");
				}

				return ShaderResourceType::UNIFORM_BUFFER;
			}

			VkDescriptorType GetVkDescriptorType(SpvReflectDescriptorType type)
			{
				switch (type)
				{
				case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER;

				case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

				case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

				case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

				case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;

				case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;

				case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

				case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

				case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;

				case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;

				case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;

				case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
					return VkDescriptorType::VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid shader descriptor type!");
				}

				return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			}
		}

		VulkanShader::VulkanShader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::filesystem::path& path, ShaderCodeType codeType)
			: Shader(pDevice, type, path, codeType)
		{
			FLINT_SETUP_PROFILER();

			ResolveShaderStage();
			std::ifstream shaderFile(path, std::ios::ate | std::ios::binary);

			if (!shaderFile.is_open())
				FLINT_THROW_RUNTIME_ERROR("Submitted shader file path is invalid!");

			UI64 codeSize = shaderFile.tellg();
			shaderFile.seekg(0);

			if (codeType == ShaderCodeType::SPIR_V)
			{
				mShaderCode.resize(codeSize);
				shaderFile.read(reinterpret_cast<char*>(mShaderCode.data()), codeSize);
			}
			else if (codeType == ShaderCodeType::GLSL)
			{
				std::string codeString;
				codeString.resize(codeSize);
				shaderFile.read(codeString.data(), codeSize);
			}

			shaderFile.close();
			CreateShaderModule();
			PerformReflection();
		}

		VulkanShader::VulkanShader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::vector<UI32>& code, ShaderCodeType codeType)
			: Shader(pDevice, type, code, codeType)
		{
			FLINT_SETUP_PROFILER();

			ResolveShaderStage();

			if (codeType != ShaderCodeType::SPIR_V) // TODO
				FLINT_THROW_RUNTIME_ERROR("Invalid shader code type!");

			mShaderCode = code;
			CreateShaderModule();
			PerformReflection();
		}

		VulkanShader::VulkanShader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::string& code, ShaderCodeType codeType)
			: Shader(pDevice, type, code, codeType)
		{
			FLINT_SETUP_PROFILER();

			ResolveShaderStage();
			if (codeType != ShaderCodeType::SPIR_V) // TODO
				FLINT_THROW_RUNTIME_ERROR("Invalid shader code type!");

			mShaderCode.resize(code.size());
			std::copy(code.begin(), code.end(), reinterpret_cast<char*>(mShaderCode.data()));

			CreateShaderModule();
			PerformReflection();
		}

		void VulkanShader::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vkDestroyShaderModule(vDevice.GetLogicalDevice(), vModule, nullptr);
		}

		VkPipelineShaderStageCreateInfo VulkanShader::GetShaderStageCreateInfo() const
		{
			VkPipelineShaderStageCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.pSpecializationInfo = VK_NULL_HANDLE;
			vCreateInfo.pName = "main";
			vCreateInfo.stage = static_cast<VkShaderStageFlagBits>(vStageFlags);
			vCreateInfo.module = vModule;

			return vCreateInfo;
		}

		void VulkanShader::PerformReflection()
		{
			FLINT_SETUP_PROFILER();

			SpvReflectShaderModule sShaderModule = {};
			UI32 variableCount = 0;

			mShaderCode = _Helpers::ResolvePadding(mShaderCode);
			_Helpers::ValidateReflection(spvReflectCreateShaderModule(mShaderCode.size() * sizeof(UI32), mShaderCode.data(), &sShaderModule));

			// Resolve shader inputs.
			{
				_Helpers::ValidateReflection(spvReflectEnumerateInputVariables(&sShaderModule, &variableCount, nullptr));

				std::vector<SpvReflectInterfaceVariable*> pInputs(variableCount);
				_Helpers::ValidateReflection(spvReflectEnumerateInputVariables(&sShaderModule, &variableCount, pInputs.data()));

				for (auto& resource : pInputs)
				{
					if (resource->format == SpvReflectFormat::SPV_REFLECT_FORMAT_UNDEFINED)
						continue;

					mInputAttributes[0].push_back(ShaderAttribute(
						resource->name,
						resource->location,
						static_cast<ShaderAttributeDataType>(
							(resource->type_description->traits.numeric.scalar.width / 8) *
							resource->type_description->traits.numeric.vector.component_count)));
				}
			}

			// Resolve shader outputs.
			{
				_Helpers::ValidateReflection(spvReflectEnumerateOutputVariables(&sShaderModule, &variableCount, nullptr));

				std::vector<SpvReflectInterfaceVariable*> pOutputs(variableCount);
				_Helpers::ValidateReflection(spvReflectEnumerateOutputVariables(&sShaderModule, &variableCount, pOutputs.data()));
				for (auto& resource : pOutputs)
				{
					if (resource->format == SpvReflectFormat::SPV_REFLECT_FORMAT_UNDEFINED)
						continue;

					mOutputAttributes[0].push_back(ShaderAttribute(
						resource->name,
						resource->location,
						static_cast<ShaderAttributeDataType>(
							(resource->type_description->traits.numeric.scalar.width / 8) *
							resource->type_description->traits.numeric.vector.component_count)));
				}
			}


			// Resolve uniforms.
			{
				_Helpers::ValidateReflection(spvReflectEnumerateDescriptorBindings(&sShaderModule, &variableCount, nullptr));

				std::vector<SpvReflectDescriptorBinding*> pBindings(variableCount);
				_Helpers::ValidateReflection(spvReflectEnumerateDescriptorBindings(&sShaderModule, &variableCount, pBindings.data()));

				VkDescriptorSetLayoutBinding vBinding = {};
				vBinding.stageFlags = vStageFlags;
				vBinding.pImmutableSamplers = VK_NULL_HANDLE;

				VkDescriptorPoolSize vSize = {};
				for (auto& resource : pBindings)
				{
					vBinding.descriptorType = _Helpers::GetVkDescriptorType(resource->descriptor_type);
					vBinding.descriptorCount = resource->count;
					vBinding.binding = resource->binding;
					INSERT_INTO_VECTOR(mBindings, vBinding);

					vSize.type = vBinding.descriptorType;
					vSize.descriptorCount = resource->count;
					INSERT_INTO_VECTOR(mSizes, vSize);

					mResources[resource->name] = ShaderResource(vBinding.binding, resource->set, _Helpers::GetShaderResourceType(resource->descriptor_type));
				}
			}

			// Resolve push constants.
			{
				_Helpers::ValidateReflection(spvReflectEnumeratePushConstantBlocks(&sShaderModule, &variableCount, nullptr));

				std::vector<SpvReflectBlockVariable*> pPushConstants(variableCount);
				_Helpers::ValidateReflection(spvReflectEnumeratePushConstantBlocks(&sShaderModule, &variableCount, pPushConstants.data()));

				VkPushConstantRange vPushConstantRange = {};
				vPushConstantRange.stageFlags = vStageFlags;
				vPushConstantRange.offset = 0;
				for (auto& resource : pPushConstants)
				{
					vPushConstantRange.size = resource->size;
					vPushConstantRange.offset = resource->offset;
					INSERT_INTO_VECTOR(mConstantRanges, vPushConstantRange);
				}
			}
		}

		void VulkanShader::ResolveShaderStage()
		{
			vStageFlags = Utilities::GetShaderStage(mType);
		}

		void VulkanShader::CreateShaderModule()
		{
			VkShaderModuleCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.codeSize = mShaderCode.size();
			vCreateInfo.pCode = mShaderCode.data();

			FLINT_VK_ASSERT(vkCreateShaderModule(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), &vCreateInfo, nullptr, &vModule));
		}
	}
}