// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanResourcePackager.hpp"
#include "VulkanBackend/VulkanShader.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanResourcePackager::VulkanResourcePackager(const uint32_t setIndex, const VulkanGraphicsPipeline* pPipeline, const VkDescriptorSetLayout vLayout)
			: ResourcePackager(GetDevice(), setIndex), vDescriptorSetLayout(vLayout)
		{
			// Resolve vertex shader data.
			{
				VulkanShader& vShader = *pPipeline->GetVertexShader();

				const auto resourceMap = vShader.GetShaderResources();
				if (!resourceMap.empty())
				{
					const auto resources = resourceMap.at(m_SetIndex);
					m_Resources.insert(resources.begin(), resources.end());
				}

				const auto descriptorSetMap = vShader.GetDescriptorSetMap();
				if (!descriptorSetMap.empty())
				{
					const auto poolSizes = descriptorSetMap.at(m_SetIndex).m_PoolSizes;
					vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
				}
			}

			// Check and resolve fragment shader data.
			if (pPipeline->GetFragmentShader())
			{
				VulkanShader& vShader = *pPipeline->GetFragmentShader();

				const auto resourceMap = vShader.GetShaderResources();
				if (!resourceMap.empty())
				{
					const auto resources = resourceMap.at(m_SetIndex);
					m_Resources.insert(resources.begin(), resources.end());
				}

				const auto descriptorSetMap = vShader.GetDescriptorSetMap();
				if (!descriptorSetMap.empty())
				{
					const auto poolSizes = descriptorSetMap.at(m_SetIndex).m_PoolSizes;
					vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
				}
			}

			// Check and resolve tessellation control shader data.
			if (pPipeline->GetTessellationControlShader())
			{
				VulkanShader& vShader = *pPipeline->GetTessellationControlShader();

				const auto resourceMap = vShader.GetShaderResources();
				if (!resourceMap.empty())
				{
					const auto resources = resourceMap.at(m_SetIndex);
					m_Resources.insert(resources.begin(), resources.end());
				}

				const auto descriptorSetMap = vShader.GetDescriptorSetMap();
				if (!descriptorSetMap.empty())
				{
					const auto poolSizes = descriptorSetMap.at(m_SetIndex).m_PoolSizes;
					vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
				}
			}

			// Check and resolve tessellation evaluation shader data.
			if (pPipeline->GetTessellationEvaluationShader())
			{
				VulkanShader& vShader = *pPipeline->GetTessellationEvaluationShader();

				const auto resourceMap = vShader.GetShaderResources();
				if (!resourceMap.empty())
				{
					const auto resources = resourceMap.at(m_SetIndex);
					m_Resources.insert(resources.begin(), resources.end());
				}

				const auto descriptorSetMap = vShader.GetDescriptorSetMap();
				if (!descriptorSetMap.empty())
				{
					const auto poolSizes = descriptorSetMap.at(m_SetIndex).m_PoolSizes;
					vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
				}
			}

			// Check and resolve geometry shader data.
			if (pPipeline->GetGeometryShader())
			{
				VulkanShader& vShader = *pPipeline->GetGeometryShader();

				const auto resourceMap = vShader.GetShaderResources();
				if (!resourceMap.empty())
				{
					const auto resources = resourceMap.at(m_SetIndex);
					m_Resources.insert(resources.begin(), resources.end());
				}

				const auto descriptorSetMap = vShader.GetDescriptorSetMap();
				if (!descriptorSetMap.empty())
				{
					const auto poolSizes = descriptorSetMap.at(m_SetIndex).m_PoolSizes;
					vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
				}
			}
		}

		VulkanResourcePackager::VulkanResourcePackager(const uint32_t setIndex, const VulkanComputePipeline* pPipeline, const VkDescriptorSetLayout vLayout)
			: ResourcePackager(GetDevice(), setIndex), vDescriptorSetLayout(vLayout)
		{
			VulkanShader& vShader = *pPipeline->GetShader();

			const auto resourceMap = vShader.GetShaderResources();
			if (!resourceMap.empty())
			{
				const auto resources = resourceMap.at(m_SetIndex);
				m_Resources.insert(resources.begin(), resources.end());
			}

			const auto descriptorSetMap = vShader.GetDescriptorSetMap();
			if (!descriptorSetMap.empty())
			{
				const auto poolSizes = descriptorSetMap.at(m_SetIndex).m_PoolSizes;
				vPoolSizes.insert(vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
			}
		}

		std::shared_ptr<VulkanResourcePackage> VulkanResourcePackager::CreatePackage()
		{
			m_DescriptorSetCount++;
			CreateDescriptorPool();
			return CreateResourcePackage();
		}

		void VulkanResourcePackager::Terminate()
		{
			VulkanDevice& vDevice = GetDevice()->StaticCast<VulkanDevice>();

			if (vDescriptorPool)
				GetDevice()->GetDeviceTable().vkDestroyDescriptorPool(GetDevice()->GetLogicalDevice(), vDescriptorPool, nullptr);
			
			bIsTerminated = true;
		}

		void VulkanResourcePackager::CreateDescriptorPool()
		{
			if (vDescriptorPool)
				GetDevice()->GetDeviceTable().vkDestroyDescriptorPool(GetDevice()->GetLogicalDevice(), vDescriptorPool, nullptr);

			VkDescriptorPoolCreateInfo vPoolCreateInfo = {};
			vPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			vPoolCreateInfo.pNext = VK_NULL_HANDLE;
			vPoolCreateInfo.flags = 0;
			vPoolCreateInfo.maxSets = m_DescriptorSetCount;
			vPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(vPoolSizes.size());
			vPoolCreateInfo.pPoolSizes = vPoolSizes.data();

			FLINT_VK_ASSERT(GetDevice()->GetDeviceTable().vkCreateDescriptorPool(GetDevice()->GetLogicalDevice(), &vPoolCreateInfo, nullptr, &vDescriptorPool));
		}

		std::shared_ptr<VulkanResourcePackage> VulkanResourcePackager::CreateResourcePackage()
		{
			std::vector<uint32_t> buffers, images;
			for (const auto binding : m_Resources)
			{
				if (binding.second == ShaderResourceType::Sampler ||
					binding.second == ShaderResourceType::SampledImage ||
					binding.second == ShaderResourceType::StorageImage ||
					binding.second == ShaderResourceType::CombinedImageSampler)
					images.emplace_back(binding.first);
				else
					buffers.emplace_back(binding.first);
			}

			std::vector<VkDescriptorSet> vDescriptorSets(m_DescriptorSetCount);

			// Allocate the descriptor sets.
			{
				std::vector<VkDescriptorSetLayout> vDescriptorSetLayouts(m_DescriptorSetCount, vDescriptorSetLayout);

				VkDescriptorSetAllocateInfo vAllocateInfo = {};
				vAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				vAllocateInfo.pNext = VK_NULL_HANDLE;
				vAllocateInfo.descriptorPool = vDescriptorPool;
				vAllocateInfo.descriptorSetCount = m_DescriptorSetCount;
				vAllocateInfo.pSetLayouts = vDescriptorSetLayouts.data();

				FLINT_VK_ASSERT(GetDevice()->GetDeviceTable().vkAllocateDescriptorSets(GetDevice()->GetLogicalDevice(), &vAllocateInfo, vDescriptorSets.data()));
			}

			// Update the existing packages.
			for (uint64_t i = 0; i < pPackages.size(); i++)
				pPackages[i]->StaticCast<VulkanResourcePackage>().SetDescriptorSet(vDescriptorSets[i]);

			// Create the new package.
			auto pNewPackage = std::make_shared<VulkanResourcePackage>(this, buffers, images, vDescriptorSets.back());
			pPackages.emplace_back(pNewPackage);

			return pNewPackage;
		}
	}
}