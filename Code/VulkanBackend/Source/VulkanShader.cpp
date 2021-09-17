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
		namespace Helpers
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
					throw backend_error("Shader parse failed!");

				case SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED:
					throw backend_error("Shader allocation failed!");

				case SPV_REFLECT_RESULT_ERROR_RANGE_EXCEEDED:
					throw backend_error("Shader range exceeded!");

				case SPV_REFLECT_RESULT_ERROR_NULL_POINTER:
					throw backend_error("Shader null pointer!");

				case SPV_REFLECT_RESULT_ERROR_INTERNAL_ERROR:
					throw backend_error("Shader internal reflection error!");

				case SPV_REFLECT_RESULT_ERROR_COUNT_MISMATCH:
					throw backend_error("Shader count mismatch!");

				case SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND:
					throw backend_error("Shader element not found!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_CODE_SIZE:
					throw backend_error("Shader invalid SPIRV code size!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_MAGIC_NUMBER:
					throw backend_error("Shader invalid SPIRV magic number!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_EOF:
					throw backend_error("Shader SPIRV unexpected end of file (EOF)!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ID_REFERENCE:
					throw backend_error("Shader invalid SPIRV ID reference!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_SET_NUMBER_OVERFLOW:
					throw backend_error("Shader invalid SPIRV descriptor set number overflow!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_STORAGE_CLASS:
					throw backend_error("Shader invalid SPIRV storage class!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_RECURSION:
					throw backend_error("Shader invalid SPIRV recursion!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_INSTRUCTION:
					throw backend_error("Shader invalid SPIRV instruction!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_BLOCK_DATA:
					throw backend_error("Shader invalid SPIRV block data!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_BLOCK_MEMBER_REFERENCE:
					throw backend_error("Shader invalid SPIRV block member reference!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ENTRY_POINT:
					throw backend_error("Shader invalid SPIRV entry point!");

				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_EXECUTION_MODE:
					throw backend_error("Shader invalid SPIRV execution mode!");
				}
			}

			ShaderResourceType GetShaderResourceType(SpvReflectDescriptorType type)
			{
				switch (type)
				{
				case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
					return ShaderResourceType::Sampler;

				case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
					return ShaderResourceType::CombinedImageSampler;

				case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
					return ShaderResourceType::SampledImage;

				case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
					return ShaderResourceType::StorageImage;

				case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
					return ShaderResourceType::UniformTexelBuffer;

				case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
					return ShaderResourceType::StorageTexelBuffer;

				case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
					return ShaderResourceType::UniformBuffer;

				case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
					return ShaderResourceType::StorageBuffer;

				case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
					return ShaderResourceType::UniformBufferDynamic;

				case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
					return ShaderResourceType::StorageBufferDynamic;

				case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
					return ShaderResourceType::InputAttachment;

				case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
					return ShaderResourceType::AccelerationStructure;

				default:
					throw backend_error("Invalid shader descriptor type!");
				}

				return ShaderResourceType::UniformBuffer;
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
					throw backend_error("Invalid shader descriptor type!");
				}

				return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			}
		}

		VulkanShader::VulkanShader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::filesystem::path& path)
			: Shader(pDevice, type, path)
		{
			FLINT_SETUP_PROFILER();

			ResolveShaderStage();
			std::ifstream shaderFile(path, std::ios::ate | std::ios::binary);

			if (!shaderFile.is_open())
				FLINT_THROW_RUNTIME_ERROR("Submitted shader file path is invalid!");

			UI64 codeSize = shaderFile.tellg();
			shaderFile.seekg(0);

			mShaderCode.resize(codeSize);
			shaderFile.read(reinterpret_cast<char*>(mShaderCode.data()), codeSize);

			shaderFile.close();
			CreateShaderModule();
			PerformReflection();
		}

		VulkanShader::VulkanShader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::vector<UI32>& code)
			: Shader(pDevice, type, code), mShaderCode(code)
		{
			FLINT_SETUP_PROFILER();

			ResolveShaderStage();

			// Add padding if the code isnt a multiple of 4.
			for (UI8 i = 0; i < mShaderCode.size() % 4; i++)
				mShaderCode.push_back(0);

			CreateShaderModule();
			PerformReflection();
		}

		VulkanShader::VulkanShader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::string& code)
			: Shader(pDevice, type, code)
		{
			FLINT_SETUP_PROFILER();

			ResolveShaderStage();

			mShaderCode.resize(code.size());
			std::copy(code.begin(), code.end(), reinterpret_cast<char*>(mShaderCode.data()));

			CreateShaderModule();
			PerformReflection();
		}

		void VulkanShader::Reload(const std::filesystem::path& path)
		{
			FLINT_SETUP_PROFILER();
			Terminate();
			bIsTerminated = false;

			ResolveShaderStage();
			std::ifstream shaderFile(path, std::ios::ate | std::ios::binary);

			if (!shaderFile.is_open())
				FLINT_THROW_RUNTIME_ERROR("Submitted shader file path is invalid!");

			UI64 codeSize = shaderFile.tellg();
			shaderFile.seekg(0);

			mShaderCode.resize(codeSize);
			shaderFile.read(reinterpret_cast<char*>(mShaderCode.data()), codeSize);

			shaderFile.close();
			CreateShaderModule();
			PerformReflection();
		}

		void VulkanShader::Reload(const std::vector<UI32>& code)
		{
			FLINT_SETUP_PROFILER();
			Terminate();
			bIsTerminated = false;

			ResolveShaderStage();

			mShaderCode = code;
			CreateShaderModule();
			PerformReflection();
		}

		void VulkanShader::Reload(const std::string& code)
		{
			FLINT_SETUP_PROFILER();
			Terminate();
			bIsTerminated = false;

			ResolveShaderStage();

			mShaderCode.resize(code.size());
			std::copy(code.begin(), code.end(), reinterpret_cast<char*>(mShaderCode.data()));

			CreateShaderModule();
			PerformReflection();
		}

		void VulkanShader::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vkDestroyShaderModule(vDevice.GetLogicalDevice(), vModule, nullptr);

			bIsTerminated = true;
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

			//std::vector<UI32> shaderCode = mShaderCode;
			std::vector<UI32> shaderCode = std::move(Helpers::ResolvePadding(mShaderCode));
			Helpers::ValidateReflection(spvReflectCreateShaderModule(shaderCode.size() * sizeof(UI32), shaderCode.data(), &sShaderModule));

			// Resolve shader inputs.
			{
				Helpers::ValidateReflection(spvReflectEnumerateInputVariables(&sShaderModule, &variableCount, nullptr));

				std::vector<SpvReflectInterfaceVariable*> pInputs(variableCount);
				Helpers::ValidateReflection(spvReflectEnumerateInputVariables(&sShaderModule, &variableCount, pInputs.data()));

				mInputAttributes[0].reserve(pInputs.size());
				for (auto& resource : pInputs)
				{
					if (resource->format == SpvReflectFormat::SPV_REFLECT_FORMAT_UNDEFINED)
						continue;

					std::string name = "";
					if (resource->name)
						name = resource->name;

					if (resource->built_in > -1)
					{
						mInputAttributes[0].push_back(ShaderAttribute(
							name,
							resource->location,
							ShaderAttributeDataType::BUILT_IN));
					}
					else
					{
						mInputAttributes[0].push_back(ShaderAttribute(
							name,
							resource->location,
							static_cast<ShaderAttributeDataType>(
								(resource->type_description->traits.numeric.scalar.width / 8) *
								std::max(resource->type_description->traits.numeric.vector.component_count, UI32(1)))));
					}
				}
			}

			// Sort the inputs.
			std::sort(mInputAttributes[0].begin(), mInputAttributes[0].end(), [] (const ShaderAttribute& lhs, const ShaderAttribute& rhs) { return lhs.mLocation < rhs.mLocation; });

			// Resolve shader outputs.
			{
				Helpers::ValidateReflection(spvReflectEnumerateOutputVariables(&sShaderModule, &variableCount, nullptr));

				std::vector<SpvReflectInterfaceVariable*> pOutputs(variableCount);
				Helpers::ValidateReflection(spvReflectEnumerateOutputVariables(&sShaderModule, &variableCount, pOutputs.data()));

				for (auto& resource : pOutputs)
				{
					if (resource->format == SpvReflectFormat::SPV_REFLECT_FORMAT_UNDEFINED)
						continue;

					std::string name = "";
					if (resource->name)
						name = resource->name;

					mOutputAttributes[0].push_back(ShaderAttribute(
						name,
						resource->location,
						static_cast<ShaderAttributeDataType>(
							(resource->type_description->traits.numeric.scalar.width / 8) *
							std::max(resource->type_description->traits.numeric.vector.component_count, UI32(1)))));
				}
			}

			// Sort the outputs.
			std::sort(mOutputAttributes[0].begin(), mOutputAttributes[0].end(), [](const ShaderAttribute& lhs, const ShaderAttribute& rhs) { return lhs.mLocation < rhs.mLocation; });

			// Resolve uniforms.
			{
				Helpers::ValidateReflection(spvReflectEnumerateDescriptorBindings(&sShaderModule, &variableCount, nullptr));

				std::vector<SpvReflectDescriptorBinding*> pBindings(variableCount);
				Helpers::ValidateReflection(spvReflectEnumerateDescriptorBindings(&sShaderModule, &variableCount, pBindings.data()));

				VkDescriptorSetLayoutBinding vBinding = {};
				vBinding.stageFlags = vStageFlags;
				vBinding.pImmutableSamplers = VK_NULL_HANDLE;

				VkDescriptorPoolSize vSize = {};
				for (auto& resource : pBindings)
				{
					vBinding.descriptorType = Helpers::GetVkDescriptorType(resource->descriptor_type);
					vBinding.descriptorCount = resource->count;
					vBinding.binding = resource->binding;

					vSize.type = vBinding.descriptorType;
					vSize.descriptorCount = resource->count;

					mDescriptorSetMap[resource->set].mLayoutBindings.push_back(vBinding);
					mDescriptorSetMap[resource->set].mPoolSizes.push_back(vSize);

					ShaderResourceKey key{ resource->set,vBinding.binding };
					mResourceMap[resource->set][resource->binding] = Helpers::GetShaderResourceType(resource->descriptor_type);
				}
			}

			// Resolve push constants.
			{
				Helpers::ValidateReflection(spvReflectEnumeratePushConstantBlocks(&sShaderModule, &variableCount, nullptr));

				std::vector<SpvReflectBlockVariable*> pPushConstants(variableCount);
				Helpers::ValidateReflection(spvReflectEnumeratePushConstantBlocks(&sShaderModule, &variableCount, pPushConstants.data()));

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