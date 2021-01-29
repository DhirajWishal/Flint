// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/RenderTargets/Pipelines/VulkanGraphicsPipeline.h"
#include "VulkanBackend/VulkanShaderModule.h"
#include "VulkanBackend/VulkanUtilities.h"
#include "VulkanBackend/VulkanMacros.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanGraphicsPipeline::Initialize(VulkanDevice* pDevice, VulkanRenderTarget* pRenderTarget, const std::vector<ShaderDigest>& shaderDigest)
		{
			this->pDevice = pDevice;
			mDigests = mDigests;

			std::vector<VkVertexInputAttributeDescription> vAttributeDesc = {};
			VkVertexInputBindingDescription vDesc = {};

			// Create the descriptor set layout.
			{
				std::vector<VkDescriptorSetLayoutBinding> vBindings = {};
				std::vector<VkDescriptorPoolSize> vPools = {};
				for (auto itr = shaderDigest.begin(); itr != shaderDigest.end(); itr++)
				{
					VulkanShaderModule sModule = {};
					sModule.Initialize(pDevice, *itr);
					vBindings.insert(vBindings.end(), sModule.mBindings.begin(), sModule.mBindings.end());
					vPools.insert(vPools.end(), sModule.mPoolSizes.begin(), sModule.mPoolSizes.end());

					if (itr->mLocation == ShaderLocation::VERTEX)
					{
						vDesc.binding = 0;
						vDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
						vDesc.stride = static_cast<UI32>(Utilities::GetStride(*itr));

						vAttributeDesc = std::move(Utilities::GetInputAttributeDescriptions(*itr));
					}
				}

				VkDescriptorSetLayoutCreateInfo vCI = {};
				vCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				vCI.flags = VK_NULL_HANDLE;
				vCI.pNext = VK_NULL_HANDLE;
				vCI.bindingCount = static_cast<UI32>(vBindings.size());
				vCI.pBindings = vBindings.data();

				FLINT_VK_ASSERT(pDevice->CreateDescriptorSetLayout(&vCI, &vSetLayout), "Failed to create descriptor set layout!")
			}
		}

		void VulkanGraphicsPipeline::Terminate()
		{
		}
	}
}