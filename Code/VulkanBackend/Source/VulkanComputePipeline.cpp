// Copyright 2021 Dhiraj Wishal
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
		VulkanComputePipeline::VulkanComputePipeline(const std::shared_ptr<Device>& pDevice, const std::string& pipelineName, const std::shared_ptr<Shader>& pComputeShader)
			: ComputePipeline(pDevice, pipelineName, pComputeShader)
		{
			CreatePipelineCache();
			CreatePipelineLayout();
			CreatePipeline();
		}

		void VulkanComputePipeline::ReloadShaders()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();

			vDevice.GetDeviceTable().vkDestroyPipeline(vDevice.GetLogicalDevice(), vPipeline, nullptr);

			for (const auto vLayout : vDescriptorSetLayouts)
				vDevice.GetDeviceTable().vkDestroyDescriptorSetLayout(vDevice.GetLogicalDevice(), vLayout, nullptr);

			vDescriptorSetLayouts.clear();
			pResourcePackagers.clear();

			vDevice.GetDeviceTable().vkDestroyPipelineLayout(vDevice.GetLogicalDevice(), vPipelineLayout, nullptr);

			bShouldPrepareResources = true;

			CreatePipelineCache();
			CreatePipelineLayout();
			CreatePipeline();
			CreateResourcePackagers();
		}

		void VulkanComputePipeline::CreateResourcePackagers()
		{
			for (UI32 i = 0; i < vDescriptorSetLayouts.size(); i++)
				pResourcePackagers.push_back(std::make_shared<VulkanResourcePackager>(i, shared_from_this(), vDescriptorSetLayouts[i]));
		}

		void VulkanComputePipeline::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();

			// Write cache data.
			UI64 cacheSize = 0;
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkGetPipelineCacheData(vDevice.GetLogicalDevice(), vPipelineCache, &cacheSize, nullptr));

			unsigned char* pDataStore = new unsigned char[cacheSize];
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkGetPipelineCacheData(vDevice.GetLogicalDevice(), vPipelineCache, &cacheSize, pDataStore));

			WriteDataToCacheFile(cacheSize, pDataStore);
			delete[] pDataStore;

			// Delete pipeline objects.
			vDevice.GetDeviceTable().vkDestroyPipeline(vDevice.GetLogicalDevice(), vPipeline, nullptr);

			for (const auto vLayout : vDescriptorSetLayouts)
				vDevice.GetDeviceTable().vkDestroyDescriptorSetLayout(vDevice.GetLogicalDevice(), vLayout, nullptr);

			vDevice.GetDeviceTable().vkDestroyPipelineLayout(vDevice.GetLogicalDevice(), vPipelineLayout, nullptr);
			vDevice.GetDeviceTable().vkDestroyPipelineCache(vDevice.GetLogicalDevice(), vPipelineCache, nullptr);

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

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreatePipelineCache(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vPipelineCache));

			delete[] pData;
		}

		void VulkanComputePipeline::CreatePipelineLayout()
		{
			OPTICK_EVENT();

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			std::vector<VkPushConstantRange> vConstantRanges;
			std::unordered_map<UI32, DescriptorSetInfo> descriptorSetInfos;

			// Resolve compute shader data.
			VulkanShader& vShader = pShader->StaticCast<VulkanShader>();
			const auto map = vShader.GetDescriptorSetMap();
			for (const auto entry : map)
			{
				auto& info = descriptorSetInfos[entry.first];
				info.mLayoutBindings.insert(info.mLayoutBindings.end(), entry.second.mLayoutBindings.begin(), entry.second.mLayoutBindings.end());
				info.mPoolSizes.insert(info.mPoolSizes.end(), entry.second.mPoolSizes.begin(), entry.second.mPoolSizes.end());
			}

			Utilities::AddPushConstantRangesToVector(vConstantRanges, vShader);

			// Create descriptor set layouts.
			for (const auto info : descriptorSetInfos)
			{
				VkDescriptorSetLayoutCreateInfo vLayoutCreateInfo = {};
				vLayoutCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				vLayoutCreateInfo.pNext = VK_NULL_HANDLE;
				vLayoutCreateInfo.flags = 0;
				vLayoutCreateInfo.bindingCount = static_cast<UI32>(info.second.mLayoutBindings.size());
				vLayoutCreateInfo.pBindings = info.second.mLayoutBindings.data();

				VkDescriptorSetLayout vDescriptorSetLayout = VK_NULL_HANDLE;
				FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateDescriptorSetLayout(vDevice.GetLogicalDevice(), &vLayoutCreateInfo, nullptr, &vDescriptorSetLayout));

				vDescriptorSetLayouts.push_back(vDescriptorSetLayout);
			}

			// Create pipeline layout.
			VkPipelineLayoutCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.pushConstantRangeCount = static_cast<UI32>(vConstantRanges.size());
			vCreateInfo.pPushConstantRanges = vConstantRanges.data();
			vCreateInfo.setLayoutCount = static_cast<UI32>(vDescriptorSetLayouts.size());
			vCreateInfo.pSetLayouts = vDescriptorSetLayouts.data();

			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreatePipelineLayout(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vPipelineLayout));
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
			vCreateInfo.stage = pShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo();

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateComputePipelines(vDevice.GetLogicalDevice(), vPipelineCache, 1, &vCreateInfo, nullptr, &vPipeline));
		}
	}
}