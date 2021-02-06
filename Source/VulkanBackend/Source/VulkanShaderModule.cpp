// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanShaderModule.h"
#include "VulkanBackend/VulkanMacros.h"
#include "VulkanBackend/VulkanUtilities.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanShaderModule::Initialize(VulkanDevice* pDevice, const ShaderDigest& digest)
		{
			this->pDevice = pDevice;
			vShaderStage = Utilities::GetShaderStage(digest.mLocation);

			VkShaderModuleCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			vCI.flags = VK_NULL_HANDLE;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.codeSize = static_cast<UI32>(digest.mCode.size());
			vCI.pCode = digest.mCode.data();

			FLINT_VK_ASSERT(pDevice->CreateShaderModule(&vCI, &vShaderModule), "Failed to create shader module!")

				SetupResources(digest);
		}

		void VulkanShaderModule::Terminate()
		{
			pDevice->DestroyShaderModule(vShaderModule);
		}

		VkPipelineShaderStageCreateInfo VulkanShaderModule::GetStage() const
		{
			VkPipelineShaderStageCreateInfo vSSCI = {};
			vSSCI.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vSSCI.pNext = VK_NULL_HANDLE;
			vSSCI.flags = VK_NULL_HANDLE;
			vSSCI.module = vShaderModule;
			vSSCI.pName = "main";
			vSSCI.pSpecializationInfo = VK_NULL_HANDLE;
			vSSCI.stage = static_cast<VkShaderStageFlagBits>(vShaderStage);

			return vSSCI;
		}

		void VulkanShaderModule::SetupResources(const ShaderDigest& digest)
		{
			VkDescriptorSetLayoutBinding vBinding = {};
			vBinding.stageFlags = vShaderStage;

			VkPushConstantRange vRange = {};
			vRange.stageFlags = vShaderStage;
			vRange.offset = 0;

			VkDescriptorPoolSize vPoolSize = {};
			for (auto itr = digest.mUniforms.begin(); itr != digest.mUniforms.end(); itr++)
			{
				if (itr->mType == UniformType::CONSTANT)
				{
					vRange.size = static_cast<UI32>(itr->mSize);
					INSERT_INTO_VECTOR(mConstantRange, vRange);
					vRange.offset += vRange.size;

					continue;
				}

				vBinding.binding = itr->mBinding;
				vBinding.descriptorType = Utilities::GetDescriptorType(itr->mType);
				vBinding.descriptorCount = itr->mLayerCount;
				vBinding.pImmutableSamplers = nullptr;
				INSERT_INTO_VECTOR(mBindings, vBinding);

				vPoolSize.descriptorCount = vBinding.descriptorCount;
				vPoolSize.type = vBinding.descriptorType;
				INSERT_INTO_VECTOR(mPoolSizes, vPoolSize);
			}
		}
	}
}