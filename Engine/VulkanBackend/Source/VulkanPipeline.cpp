// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanPipeline.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

#include <fstream>

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanPipeline::VulkanPipeline(VulkanDevice& device, const std::filesystem::path& cacheFile)
			: m_CacheFile(cacheFile), m_Device(device)
		{
			// Load the cache if possible.
			loadCache();
		}

		VulkanPipeline::~VulkanPipeline()
		{
			// Save the cache before we leave.
			saveCache();

			// Destroy the resources.
			m_Device.getDeviceTable().vkDestroyPipeline(m_Device.getLogicalDevice(), m_Pipeline, nullptr);
			m_Device.getDeviceTable().vkDestroyPipelineCache(m_Device.getLogicalDevice(), m_PipelineCache, nullptr);
			m_Device.getDeviceTable().vkDestroyPipelineLayout(m_Device.getLogicalDevice(), m_PipelineLayout, nullptr);
		}

		void VulkanPipeline::loadCache()
		{
			// Load data from file.
			std::fstream cacheFile(m_CacheFile, std::ios::in | std::ios::ate | std::ios::binary);

			uint64_t size = 0;
			std::unique_ptr<uint8_t[]> buffer;

			// If file does not exist, return without an issue.
			if (cacheFile.is_open())
			{
				size = cacheFile.tellg();
				cacheFile.seekg(0);

				buffer = std::make_unique<uint8_t[]>(size);
				cacheFile.read(reinterpret_cast<char*>(buffer.get()), size);

				cacheFile.close();
			}

			// Create the pipeline cache.
			VkPipelineCacheCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			createInfo.pNext = VK_NULL_HANDLE;
			createInfo.flags = 0;
			createInfo.initialDataSize = size;
			createInfo.pInitialData = buffer.get();

			FLINT_VK_ASSERT(m_Device.getDeviceTable().vkCreatePipelineCache(m_Device.getLogicalDevice(), &createInfo, nullptr, &m_PipelineCache), "Failed to create the pipeline cache!");
		}

		void VulkanPipeline::saveCache()
		{
			// Return if we don't have anything to save.
			if (m_PipelineCache == VK_NULL_HANDLE)
				return;

			// Load cache data.
			size_t cacheSize = 0;
			FLINT_VK_ASSERT(m_Device.getDeviceTable().vkGetPipelineCacheData(m_Device.getLogicalDevice(), m_PipelineCache, &cacheSize, nullptr), "Failed to get the pipeline cache size!");

			auto buffer = std::make_unique<uint8_t[]>(cacheSize);
			FLINT_VK_ASSERT(m_Device.getDeviceTable().vkGetPipelineCacheData(m_Device.getLogicalDevice(), m_PipelineCache, &cacheSize, buffer.get()), "Failed to get the pipeline cache data!");

			// Write to file.
			std::fstream cacheFile(m_CacheFile, std::ios::out | std::ios::binary);

			// Write the data if we were able to open the file.
			if (cacheFile.is_open())
				cacheFile.write(reinterpret_cast<char*>(buffer.get()), cacheSize);

			cacheFile.flush();
			cacheFile.close();
		}
	}
}