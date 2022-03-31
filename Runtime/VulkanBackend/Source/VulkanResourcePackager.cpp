// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanResourcePackager.hpp"
#include "VulkanBackend/VulkanShader.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanResourcePackager::VulkanResourcePackager(const uint32_t setIndex, const VulkanGraphicsPipeline* pPipeline, const VkDescriptorSetLayout m_vLayout)
			: ResourcePackager(GetDevice(), setIndex), m_vDescriptorSetLayout(m_vLayout)
		{
			// Resolve vertex shader data.
			{
				VulkanShader& m_vShader = *pPipeline->GetVertexShader();

				const auto resourceMap = m_vShader.GetShaderResources();
				if (!resourceMap.empty())
				{
					const auto resources = resourceMap.at(m_SetIndex);
					m_Resources.insert(resources.begin(), resources.end());
				}

				const auto descriptorSetMap = m_vShader.GetDescriptorSetMap();
				if (!descriptorSetMap.empty())
				{
					const auto poolSizes = descriptorSetMap.at(m_SetIndex).m_PoolSizes;
					m_vPoolSizes.insert(m_vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
				}
			}

			// Check and resolve fragment shader data.
			if (pPipeline->GetFragmentShader())
			{
				VulkanShader& m_vShader = *pPipeline->GetFragmentShader();

				const auto resourceMap = m_vShader.GetShaderResources();
				if (!resourceMap.empty())
				{
					const auto resources = resourceMap.at(m_SetIndex);
					m_Resources.insert(resources.begin(), resources.end());
				}

				const auto descriptorSetMap = m_vShader.GetDescriptorSetMap();
				if (!descriptorSetMap.empty())
				{
					const auto poolSizes = descriptorSetMap.at(m_SetIndex).m_PoolSizes;
					m_vPoolSizes.insert(m_vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
				}
			}

			// Check and resolve tessellation control shader data.
			if (pPipeline->GetTessellationControlShader())
			{
				VulkanShader& m_vShader = *pPipeline->GetTessellationControlShader();

				const auto resourceMap = m_vShader.GetShaderResources();
				if (!resourceMap.empty())
				{
					const auto resources = resourceMap.at(m_SetIndex);
					m_Resources.insert(resources.begin(), resources.end());
				}

				const auto descriptorSetMap = m_vShader.GetDescriptorSetMap();
				if (!descriptorSetMap.empty())
				{
					const auto poolSizes = descriptorSetMap.at(m_SetIndex).m_PoolSizes;
					m_vPoolSizes.insert(m_vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
				}
			}

			// Check and resolve tessellation evaluation shader data.
			if (pPipeline->GetTessellationEvaluationShader())
			{
				VulkanShader& m_vShader = *pPipeline->GetTessellationEvaluationShader();

				const auto resourceMap = m_vShader.GetShaderResources();
				if (!resourceMap.empty())
				{
					const auto resources = resourceMap.at(m_SetIndex);
					m_Resources.insert(resources.begin(), resources.end());
				}

				const auto descriptorSetMap = m_vShader.GetDescriptorSetMap();
				if (!descriptorSetMap.empty())
				{
					const auto poolSizes = descriptorSetMap.at(m_SetIndex).m_PoolSizes;
					m_vPoolSizes.insert(m_vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
				}
			}

			// Check and resolve geometry shader data.
			if (pPipeline->GetGeometryShader())
			{
				VulkanShader& m_vShader = *pPipeline->GetGeometryShader();

				const auto resourceMap = m_vShader.GetShaderResources();
				if (!resourceMap.empty())
				{
					const auto resources = resourceMap.at(m_SetIndex);
					m_Resources.insert(resources.begin(), resources.end());
				}

				const auto descriptorSetMap = m_vShader.GetDescriptorSetMap();
				if (!descriptorSetMap.empty())
				{
					const auto poolSizes = descriptorSetMap.at(m_SetIndex).m_PoolSizes;
					m_vPoolSizes.insert(m_vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
				}
			}
		}

		VulkanResourcePackager::VulkanResourcePackager(const uint32_t setIndex, const VulkanComputePipeline* pPipeline, const VkDescriptorSetLayout m_vLayout)
			: ResourcePackager(GetDevice(), setIndex), m_vDescriptorSetLayout(m_vLayout)
		{
			VulkanShader& m_vShader = *pPipeline->GetShader();

			const auto resourceMap = m_vShader.GetShaderResources();
			if (!resourceMap.empty())
			{
				const auto resources = resourceMap.at(m_SetIndex);
				m_Resources.insert(resources.begin(), resources.end());
			}

			const auto descriptorSetMap = m_vShader.GetDescriptorSetMap();
			if (!descriptorSetMap.empty())
			{
				const auto poolSizes = descriptorSetMap.at(m_SetIndex).m_PoolSizes;
				m_vPoolSizes.insert(m_vPoolSizes.end(), poolSizes.begin(), poolSizes.end());
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
			VulkanDevice& m_vDevice = GetDevice()->StaticCast<VulkanDevice>();

			if (m_vDescriptorPool)
				GetDevice()->GetDeviceTable().vkDestroyDescriptorPool(GetDevice()->GetLogicalDevice(), m_vDescriptorPool, nullptr);
			
			bIsTerminated = true;
		}

		void VulkanResourcePackager::CreateDescriptorPool()
		{
			if (m_vDescriptorPool)
				GetDevice()->GetDeviceTable().vkDestroyDescriptorPool(GetDevice()->GetLogicalDevice(), m_vDescriptorPool, nullptr);

			VkDescriptorPoolCreateInfo m_vPoolCreateInfo = {};
			m_vPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			m_vPoolCreateInfo.pNext = VK_NULL_HANDLE;
			m_vPoolCreateInfo.flags = 0;
			m_vPoolCreateInfo.maxSets = m_DescriptorSetCount;
			m_vPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(m_vPoolSizes.size());
			m_vPoolCreateInfo.pPoolSizes = m_vPoolSizes.data();

			FLINT_VK_ASSERT(GetDevice()->GetDeviceTable().vkCreateDescriptorPool(GetDevice()->GetLogicalDevice(), &m_vPoolCreateInfo, nullptr, &m_vDescriptorPool));
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

			std::vector<VkDescriptorSet> m_vDescriptorSets(m_DescriptorSetCount);

			// Allocate the descriptor sets.
			{
				std::vector<VkDescriptorSetLayout> m_vDescriptorSetLayouts(m_DescriptorSetCount, m_vDescriptorSetLayout);

				VkDescriptorSetAllocateInfo m_vAllocateInfo = {};
				m_vAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				m_vAllocateInfo.pNext = VK_NULL_HANDLE;
				m_vAllocateInfo.descriptorPool = m_vDescriptorPool;
				m_vAllocateInfo.descriptorSetCount = m_DescriptorSetCount;
				m_vAllocateInfo.pSetLayouts = m_vDescriptorSetLayouts.data();

				FLINT_VK_ASSERT(GetDevice()->GetDeviceTable().vkAllocateDescriptorSets(GetDevice()->GetLogicalDevice(), &m_vAllocateInfo, m_vDescriptorSets.data()));
			}

			// Update the existing packages.
			for (uint64_t i = 0; i < pPackages.size(); i++)
				pPackages[i]->StaticCast<VulkanResourcePackage>().SetDescriptorSet(m_vDescriptorSets[i]);

			// Create the new package.
			auto pNewPackage = std::make_shared<VulkanResourcePackage>(this, buffers, images, m_vDescriptorSets.back());
			pPackages.emplace_back(pNewPackage);

			return pNewPackage;
		}
	}
}