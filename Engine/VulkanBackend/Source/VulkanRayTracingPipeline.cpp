// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanRayTracingPipeline.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanRayTracingPipeline::VulkanRayTracingPipeline(const std::shared_ptr<VulkanDevice>& pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler /*= nullptr*/)
			: RayTracingPipeline(pDevice, std::move(pCacheHandler))
			, VulkanPipeline(pDevice)
		{
			// Load the cache if possible.
			loadCache();

			// Make sure to set the object as valid.
			validate();
		}

		VulkanRayTracingPipeline::~VulkanRayTracingPipeline()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanRayTracingPipeline::terminate()
		{
			saveCache();
			invalidate();
		}

		void VulkanRayTracingPipeline::loadCache()
		{
			std::vector<std::byte> buffer;

			// Load the cache if possible.
			if (m_pCacheHandler)
				buffer = m_pCacheHandler->load();

			// Create the pipeline cache.
			VkPipelineCacheCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			createInfo.pNext = VK_NULL_HANDLE;
			createInfo.flags = 0;
			createInfo.initialDataSize = buffer.size();
			createInfo.pInitialData = buffer.data();

			FLINT_VK_ASSERT(getDevicePointerAs<VulkanDevice>()->getDeviceTable().vkCreatePipelineCache(getDevicePointerAs<VulkanDevice>()->getLogicalDevice(), &createInfo, nullptr, &m_PipelineCache), "Failed to create the pipeline cache!");
		}

		void VulkanRayTracingPipeline::saveCache()
		{
			// Return if we don't have anything to save.
			if (m_PipelineCache == VK_NULL_HANDLE)
				return;

			// Load cache data.
			size_t cacheSize = 0;
			FLINT_VK_ASSERT(getDevicePointerAs<VulkanDevice>()->getDeviceTable().vkGetPipelineCacheData(getDevicePointerAs<VulkanDevice>()->getLogicalDevice(), m_PipelineCache, &cacheSize, nullptr), "Failed to get the pipeline cache size!");

			auto buffer = std::vector<std::byte>(cacheSize);
			FLINT_VK_ASSERT(getDevicePointerAs<VulkanDevice>()->getDeviceTable().vkGetPipelineCacheData(getDevicePointerAs<VulkanDevice>()->getLogicalDevice(), m_PipelineCache, &cacheSize, buffer.data()), "Failed to get the pipeline cache data!");

			// Store the cache if possible.
			if (m_pCacheHandler)
				m_pCacheHandler->store(buffer);
		}
	}
}