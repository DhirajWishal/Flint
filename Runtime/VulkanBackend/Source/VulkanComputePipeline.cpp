// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanComputePipeline.hpp"
#include "VulkanBackend/VulkanShader.hpp"
#include "VulkanBackend/VulkanOneTimeCommandBuffer.hpp"
#include "VulkanBackend/VulkanResourcePackager.hpp"
#include "VulkanBackend/VulkanCommandBuffer.hpp"
#include "VulkanBackend/VulkanCommandBufferAllocator.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanComputePipeline::VulkanComputePipeline(VulkanDevice* m_pDevice, const std::string& pipelineName, std::unique_ptr<VulkanShader>&& pComputeShader)
			: ComputePipeline(m_pDevice, pipelineName, std::move(pComputeShader))
		{
			CreatePipelineCache();
			CreatePipelineLayout();
			CreatePipeline();
		}

		void VulkanComputePipeline::ReloadShaders()
		{
			m_pDevice->GetDeviceTable().vkDestroyPipeline(m_pDevice->GetLogicalDevice(), m_vPipeline, nullptr);

			for (const auto m_vLayout : m_vDescriptorSetLayouts)
				m_pDevice->GetDeviceTable().vkDestroyDescriptorSetLayout(m_pDevice->GetLogicalDevice(), m_vLayout, nullptr);

			m_vDescriptorSetLayouts.clear();
			pResourcePackagers.clear();

			m_pDevice->GetDeviceTable().vkDestroyPipelineLayout(m_pDevice->GetLogicalDevice(), m_vPipelineLayout, nullptr);

			bShouldPrepareResources = true;

			CreatePipelineCache();
			CreatePipelineLayout();
			CreatePipeline();
			CreateResourcePackagers();
		}

		void VulkanComputePipeline::CreateResourcePackagers()
		{
			for (uint32_t i = 0; i < m_vDescriptorSetLayouts.size(); i++)
				pResourcePackagers.emplace_back(std::make_shared<VulkanResourcePackager>(i, this, m_vDescriptorSetLayouts[i]));
		}

		void VulkanComputePipeline::Terminate()
		{
			// Write cache data.
			uint64_t cacheSize = 0;
			FLINT_VK_ASSERT(m_pDevice->GetDeviceTable().vkGetPipelineCacheData(m_pDevice->GetLogicalDevice(), m_vPipelineCache, &cacheSize, nullptr));

			unsigned char* pDataStore = new unsigned char[cacheSize];
			FLINT_VK_ASSERT(m_pDevice->GetDeviceTable().vkGetPipelineCacheData(m_pDevice->GetLogicalDevice(), m_vPipelineCache, &cacheSize, pDataStore));

			WriteDataToCacheFile(cacheSize, pDataStore);
			delete[] pDataStore;

			// Delete pipeline objects.
			m_pDevice->GetDeviceTable().vkDestroyPipeline(m_pDevice->GetLogicalDevice(), m_vPipeline, nullptr);

			for (const auto m_vLayout : m_vDescriptorSetLayouts)
				m_pDevice->GetDeviceTable().vkDestroyDescriptorSetLayout(m_pDevice->GetLogicalDevice(), m_vLayout, nullptr);

			m_pDevice->GetDeviceTable().vkDestroyPipelineLayout(m_pDevice->GetLogicalDevice(), m_vPipelineLayout, nullptr);
			m_pDevice->GetDeviceTable().vkDestroyPipelineCache(m_pDevice->GetLogicalDevice(), m_vPipelineCache, nullptr);

			m_vDescriptorSetLayouts.clear();
			pResourcePackagers.clear();

			bIsTerminated = true;
		}

		void VulkanComputePipeline::CreatePipelineCache()
		{
			OPTICK_EVENT();

			const auto [size, pData] = ReadDataFromCacheFile();

			VkPipelineCacheCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.flags = 0;
			m_vCreateInfo.initialDataSize = size;
			m_vCreateInfo.pInitialData = pData;

			FLINT_VK_ASSERT(m_pDevice->GetDeviceTable().vkCreatePipelineCache(m_pDevice->GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vPipelineCache));

			delete[] pData;
		}

		void VulkanComputePipeline::CreatePipelineLayout()
		{
			OPTICK_EVENT();

			std::vector<VkPushConstantRange> m_vConstantRanges;
			std::unordered_map<uint32_t, DescriptorSetInfo> descriptorSetInfos;

			// Resolve compute shader data.
			const auto map = pShader->GetDescriptorSetMap();
			for (const auto entry : map)
			{
				auto& info = descriptorSetInfos[entry.first];
				info.m_LayoutBindings.insert(info.m_LayoutBindings.end(), entry.second.m_LayoutBindings.begin(), entry.second.m_LayoutBindings.end());
				info.m_PoolSizes.insert(info.m_PoolSizes.end(), entry.second.m_PoolSizes.begin(), entry.second.m_PoolSizes.end());
			}

			Utilities::AddPushConstantRangesToVector(m_vConstantRanges, *pShader);

			// Create descriptor set layouts.
			for (const auto info : descriptorSetInfos)
			{
				VkDescriptorSetLayoutCreateInfo m_vLayoutCreateInfo = {};
				m_vLayoutCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				m_vLayoutCreateInfo.pNext = VK_NULL_HANDLE;
				m_vLayoutCreateInfo.flags = 0;
				m_vLayoutCreateInfo.bindingCount = static_cast<uint32_t>(info.second.m_LayoutBindings.size());
				m_vLayoutCreateInfo.pBindings = info.second.m_LayoutBindings.data();

				VkDescriptorSetLayout m_vDescriptorSetLayout = VK_NULL_HANDLE;
				FLINT_VK_ASSERT(m_pDevice->GetDeviceTable().vkCreateDescriptorSetLayout(m_pDevice->GetLogicalDevice(), &m_vLayoutCreateInfo, nullptr, &m_vDescriptorSetLayout));

				m_vDescriptorSetLayouts.emplace_back(m_vDescriptorSetLayout);
			}

			// Create pipeline layout.
			VkPipelineLayoutCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.flags = 0;
			m_vCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(m_vConstantRanges.size());
			m_vCreateInfo.pPushConstantRanges = m_vConstantRanges.data();
			m_vCreateInfo.setLayoutCount = static_cast<uint32_t>(m_vDescriptorSetLayouts.size());
			m_vCreateInfo.pSetLayouts = m_vDescriptorSetLayouts.data();

			FLINT_VK_ASSERT(m_pDevice->GetDeviceTable().vkCreatePipelineLayout(m_pDevice->GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vPipelineLayout));
		}

		void VulkanComputePipeline::CreatePipeline()
		{
			VkComputePipelineCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.flags = 0;
			m_vCreateInfo.layout = m_vPipelineLayout;
			m_vCreateInfo.basePipelineIndex = 0;
			m_vCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			m_vCreateInfo.stage = pShader->GetShaderStageCreateInfo();

			FLINT_VK_ASSERT(m_pDevice->GetDeviceTable().vkCreateComputePipelines(m_pDevice->GetLogicalDevice(), m_vPipelineCache, 1, &m_vCreateInfo, nullptr, &m_vPipeline));
		}
	}
}