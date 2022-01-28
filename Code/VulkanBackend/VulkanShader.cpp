// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include <volk.h>
export module Flint.VulkanBackend.VulkanShader;

import Flint.VulkanBackend.VulkanDevice;

namespace Flint
{
	namespace VulkanBackend
	{
		struct DescriptorSetInfo
		{
			std::vector<VkDescriptorSetLayoutBinding> mLayoutBindings;
			std::vector<VkDescriptorPoolSize> mPoolSizes;
		};

		class VulkanShader final : public Shader, public std::enable_shared_from_this<VulkanShader>
		{
		public:
			VulkanShader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::filesystem::path& path);
			VulkanShader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::vector<uint32>& code);
			VulkanShader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::string& code);
			~VulkanShader() { if (!bIsTerminated) Terminate(); }

			virtual void CreateCache(const std::filesystem::path& path) const override;
			virtual void Reload(const std::filesystem::path& path) override;
			virtual void Reload(const std::vector<uint32>& code) override;
			virtual void Reload(const std::string& code) override;

			virtual void Terminate() override;

			VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo() const;
			void PerformReflection();

			VkShaderModule GetModule() const { return vModule; }
			const std::vector<VkPushConstantRange> GetPushConstantRanges() const { return mConstantRanges; }
			const std::unordered_map<uint32, DescriptorSetInfo> GetDescriptorSetMap() const { return mDescriptorSetMap; }

		private:
			void ResolveShaderStage();
			void CreateShaderModule();

		private:
			std::vector<uint32> mShaderCode;
			std::vector<VkPushConstantRange> mConstantRanges;

			std::unordered_map<uint32, DescriptorSetInfo> mDescriptorSetMap;

			VkShaderModule vModule = VK_NULL_HANDLE;

			VkShaderStageFlags vStageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
		};
	}
}

module: private;

import Flint.VulkanBackend.VulkanUtilities;

#include "spirv_reflect.h"
#include <fstream>

namespace Flint
{
	namespace VulkanBackend
	{
		namespace Helpers
		{
			std::vector<uint32> ResolvePadding(const std::vector<uint32>& code)
			{
				const uint64 finalCodeSize = code.size() / 4;
				std::vector<uint32> resolvedCode(code.begin(), code.begin() + finalCodeSize);

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
			OPTICK_EVENT();

			ResolveShaderStage();
			std::ifstream shaderFile(path, std::ios::in | std::ios::ate | std::ios::binary);

			if (!shaderFile.is_open())
				throw std::runtime_error("Submitted shader file path is invalid!");

			const uint64 codeSize = shaderFile.tellg();
			shaderFile.seekg(0);

			mShaderCode.resize(codeSize);
			shaderFile.read(reinterpret_cast<char*>(mShaderCode.data()), codeSize);

			shaderFile.close();
			CreateShaderModule();
			PerformReflection();
		}

		VulkanShader::VulkanShader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::vector<uint32>& code)
			: Shader(pDevice, type, code), mShaderCode(code)
		{
			OPTICK_EVENT();

			ResolveShaderStage();

			// Add padding if the code isnt a multiple of 4.
			for (uint8 i = 0; i < mShaderCode.size() % 4; i++)
				mShaderCode.emplace_back(0);

			CreateShaderModule();
			PerformReflection();
		}

		VulkanShader::VulkanShader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::string& code)
			: Shader(pDevice, type, code)
		{
			OPTICK_EVENT();

			ResolveShaderStage();

			mShaderCode.resize(code.size());
			std::copy(code.begin(), code.end(), reinterpret_cast<char*>(mShaderCode.data()));

			CreateShaderModule();
			PerformReflection();
		}

		void VulkanShader::CreateCache(const std::filesystem::path& path) const
		{
			std::ofstream cacheFile(path, std::ios::out | std::ios::binary);

			if (!cacheFile.is_open())
				throw std::runtime_error("Unable to open the provided cache file!");

			cacheFile.write(reinterpret_cast<const char*>(mShaderCode.data()), mShaderCode.size());
			cacheFile.close();
		}

		void VulkanShader::Reload(const std::filesystem::path& path)
		{
			OPTICK_EVENT();
			Terminate();
			bIsTerminated = false;

			ResolveShaderStage();
			std::ifstream shaderFile(path, std::ios::ate | std::ios::binary);

			if (!shaderFile.is_open())
				throw std::runtime_error("Submitted shader file path is invalid!");

			const uint64 codeSize = shaderFile.tellg();
			shaderFile.seekg(0);

			mShaderCode.resize(codeSize);
			shaderFile.read(reinterpret_cast<char*>(mShaderCode.data()), codeSize);

			shaderFile.close();
			CreateShaderModule();
			PerformReflection();
		}

		void VulkanShader::Reload(const std::vector<uint32>& code)
		{
			OPTICK_EVENT();
			Terminate();
			bIsTerminated = false;

			ResolveShaderStage();

			mShaderCode = code;
			CreateShaderModule();
			PerformReflection();
		}

		void VulkanShader::Reload(const std::string& code)
		{
			OPTICK_EVENT();
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
			const VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkDestroyShaderModule(vDevice.GetLogicalDevice(), vModule, nullptr);

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
			OPTICK_EVENT();

			SpvReflectShaderModule sShaderModule = {};
			uint32 variableCount = 0;

			//std::vector<uint32> shaderCode = mShaderCode;
			std::vector<uint32> shaderCode = std::move(Helpers::ResolvePadding(mShaderCode));
			Helpers::ValidateReflection(spvReflectCreateShaderModule(shaderCode.size() * sizeof(uint32), shaderCode.data(), &sShaderModule));

			// Resolve shader inputs.
			{
				Helpers::ValidateReflection(spvReflectEnumerateInputVariables(&sShaderModule, &variableCount, nullptr));

				std::vector<SpvReflectInterfaceVariable*> pInputs(variableCount);
				Helpers::ValidateReflection(spvReflectEnumerateInputVariables(&sShaderModule, &variableCount, pInputs.data()));

				mInputAttributes.reserve(pInputs.size());
				for (auto& resource : pInputs)
				{
					if (resource->format == SpvReflectFormat::SPV_REFLECT_FORMAT_UNDEFINED)
						continue;

					std::string name = "";
					if (resource->name)
						name = resource->name;

					if (resource->built_in > -1)
					{
						mInputAttributes.emplace_back(ShaderAttribute(
							name,
							resource->location,
							ShaderAttributeDataType::BUILT_IN));
					}
					else
					{
						mInputAttributes.emplace_back(ShaderAttribute(
							name,
							resource->location,
							static_cast<ShaderAttributeDataType>(
								(resource->type_description->traits.numeric.scalar.width / 8) *
								std::max(resource->type_description->traits.numeric.vector.component_count, uint32(1)))));
					}
				}
			}

			// Sort the inputs.
			std::sort(mInputAttributes.begin(), mInputAttributes.end(), [](const ShaderAttribute& lhs, const ShaderAttribute& rhs) { return lhs.mLocation < rhs.mLocation; });

			// Resolve shader outputs.
			{
				Helpers::ValidateReflection(spvReflectEnumerateOutputVariables(&sShaderModule, &variableCount, nullptr));

				std::vector<SpvReflectInterfaceVariable*> pOutputs(variableCount);
				Helpers::ValidateReflection(spvReflectEnumerateOutputVariables(&sShaderModule, &variableCount, pOutputs.data()));

				mInputAttributes.reserve(pOutputs.size());
				for (auto& resource : pOutputs)
				{
					if (resource->format == SpvReflectFormat::SPV_REFLECT_FORMAT_UNDEFINED)
						continue;

					std::string name = "";
					if (resource->name)
						name = resource->name;

					mOutputAttributes.emplace_back(ShaderAttribute(
						name,
						resource->location,
						static_cast<ShaderAttributeDataType>(
							(resource->type_description->traits.numeric.scalar.width / 8) *
							std::max(resource->type_description->traits.numeric.vector.component_count, uint32(1)))));
				}
			}

			// Sort the outputs.
			std::sort(mOutputAttributes.begin(), mOutputAttributes.end(), [](const ShaderAttribute& lhs, const ShaderAttribute& rhs) { return lhs.mLocation < rhs.mLocation; });

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

					mDescriptorSetMap[resource->set].mLayoutBindings.emplace_back(vBinding);
					mDescriptorSetMap[resource->set].mPoolSizes.emplace_back(vSize);

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
					mConstantRanges.emplace_back(vPushConstantRange);
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

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateShaderModule(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vModule));
		}
	}
}