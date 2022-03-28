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
		VulkanComputePipeline::VulkanComputePipeline(VulkanDevice* pDevice, const std::string& pipelineName, std::unique_ptr<VulkanShader>&& pComputeShader)
			: ComputePipeline(pDevice, pipelineName, std::move(pComputeShader))
		{
			CreatePipelineCache();
			CreatePipelineLayout();
			CreatePipeline();
		}

		void VulkanComputePipeline::ReloadShaders()
		{
			pDevice->GetDeviceTable().vkDestroyPipeline(pDevice->GetLogicalDevice(), vPipeline, nullptr);

			for (const auto vLayout : vDescriptorSetLayouts)
				pDevice->GetDeviceTable().vkDestroyDescriptorSetLayout(pDevice->GetLogicalDevice(), vLayout, nullptr);

			vDescriptorSetLayouts.clear();
			pResourcePackagers.clear();

			pDevice->GetDeviceTable().vkDestroyPipelineLayout(pDevice->GetLogicalDevice(), vPipelineLayout, nullptr);

			bShouldPrepareResources = true;

			CreatePipelineCache();
			CreatePipelineLayout();
			CreatePipeline();
			CreateResourcePackagers();
		}

		void VulkanComputePipeline::CreateResourcePackagers()
		{
			for (uint32_t i = 0; i < vDescriptorSetLayouts.size(); i++)
				pResourcePackagers.emplace_back(std::make_shared<VulkanResourcePackager>(i, this, vDescriptorSetLayouts[i]));
		}

		void VulkanComputePipeline::Terminate()
		{
			// Write cache data.
			uint64_t cacheSize = 0;
			FLINT_VK_ASSERT(pDevice->GetDeviceTable().vkGetPipelineCacheData(pDevice->GetLogicalDevice(), vPipelineCache, &cacheSize, nullptr));

			unsigned char* pDataStore = new unsigned char[cacheSize];
			FLINT_VK_ASSERT(pDevice->GetDeviceTable().vkGetPipelineCacheData(pDevice->GetLogicalDevice(), vPipelineCache, &cacheSize, pDataStore));

			WriteDataToCacheFile(cacheSize, pDataStore);
			delete[] pDataStore;

			// Delete pipeline objects.
			pDevice->GetDeviceTable().vkDestroyPipeline(pDevice->GetLogicalDevice(), vPipeline, nullptr);

			for (const auto vLayout : vDescriptorSetLayouts)
				pDevice->GetDeviceTable().vkDestroyDescriptorSetLayout(pDevice->GetLogicalDevice(), vLayout, nullptr);

			pDevice->GetDeviceTable().vkDestroyPipelineLayout(pDevice->GetLogicalDevice(), vPipelineLayout, nullptr);
			pDevice->GetDeviceTable().vkDestroyPipelineCache(pDevice->GetLogicalDevice(), vPipelineCache, nullptr);

			vDescriptorSetLayouts.clear();
			pResourcePackagers.clear();

			bIsTerminated = true;
		}

		void VulkanComputePipeline::CreatePipelineCache()
		{
			OPTICK_EVENT();

			const auto [size, pData] = ReadDataFromCacheFile();

			VkPipelineCacheCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.initialDataSize = size;
			vCreateInfo.pInitialData = pData;

			FLINT_VK_ASSERT(pDevice->GetDeviceTable().vkCreatePipelineCache(pDevice->GetLogicalDevice(), &vCreateInfo, nullptr, &vPipelineCache));

			delete[] pData;
		}

		void VulkanComputePipeline::CreatePipelineLayout()
		{
			OPTICK_EVENT();

			std::vector<VkPushConstantRange> vConstantRanges;
			std::unordered_map<uint32_t, DescriptorSetInfo> descriptorSetInfos;

			// Resolve compute shader data.
			const auto map = pShader->GetDescriptorSetMap();
			for (const auto entry : map)
			{
				auto& info = descriptorSetInfos[entry.first];
				info.m_LayoutBindings.insert(info.m_LayoutBindings.end(), entry.second.m_LayoutBindings.begin(), entry.second.m_LayoutBindings.end());
				info.m_PoolSizes.insert(info.m_PoolSizes.end(), entry.second.m_PoolSizes.begin(), entry.second.m_PoolSizes.end());
			}

			Utilities::AddPushConstantRangesToVector(vConstantRanges, *pShader);

			// Create descriptor set layouts.
			for (const auto info : descriptorSetInfos)
			{
				VkDescriptorSetLayoutCreateInfo vLayoutCreateInfo = {};
				vLayoutCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				vLayoutCreateInfo.pNext = VK_NULL_HANDLE;
				vLayoutCreateInfo.flags = 0;
				vLayoutCreateInfo.bindingCount = static_cast<uint32_t>(info.second.m_LayoutBindings.size());
				vLayoutCreateInfo.pBindings = info.second.m_LayoutBindings.data();

				VkDescriptorSetLayout vDescriptorSetLayout = VK_NULL_HANDLE;
				FLINT_VK_ASSERT(pDevice->GetDeviceTable().vkCreateDescriptorSetLayout(pDevice->GetLogicalDevice(), &vLayoutCreateInfo, nullptr, &vDescriptorSetLayout));

				vDescriptorSetLayouts.emplace_back(vDescriptorSetLayout);
			}

			// Create pipeline layout.
			VkPipelineLayoutCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(vConstantRanges.size());
			vCreateInfo.pPushConstantRanges = vConstantRanges.data();
			vCreateInfo.setLayoutCount = static_cast<uint32_t>(vDescriptorSetLayouts.size());
			vCreateInfo.pSetLayouts = vDescriptorSetLayouts.data();

			FLINT_VK_ASSERT(pDevice->GetDeviceTable().vkCreatePipelineLayout(pDevice->GetLogicalDevice(), &vCreateInfo, nullptr, &vPipelineLayout));
		}

		void VulkanComputePipeline::CreatePipeline()
		{
			VkComputePipelineCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.layout = vPipelineLayout;
			vCreateInfo.basePipelineIndex = 0;
			vCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			vCreateInfo.stage = pShader->GetShaderStageCreateInfo();

			FLINT_VK_ASSERT(pDevice->GetDeviceTable().vkCreateComputePipelines(pDevice->GetLogicalDevice(), vPipelineCache, 1, &vCreateInfo, nullptr, &vPipeline));
		}
	}
}