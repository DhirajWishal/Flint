// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/VulkanBackend/VulkanRayTracingPipeline.hpp"
#include "Flint/VulkanBackend/VulkanMacros.hpp"

#include <Optick.h>

namespace Flint
{
	namespace Backend
	{
		VulkanRayTracingPipeline::VulkanRayTracingPipeline(const std::shared_ptr<VulkanDevice>& pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler /*= nullptr*/)
			: RayTracingPipeline(pDevice, std::move(pCacheHandler))
		{
			// Make sure to set the object as valid.
			validate();
		}

		VulkanRayTracingPipeline::~VulkanRayTracingPipeline()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanRayTracingPipeline::terminate()
		{
			invalidate();
		}

		VkPipelineCache VulkanRayTracingPipeline::loadCache(uint64_t identifier) const
		{
			OPTICK_EVENT();

			std::vector<std::byte> buffer;

			// Load the cache if possible.
			if (m_pCacheHandler)
				buffer = m_pCacheHandler->load(identifier);

			// Create the pipeline cache.
			VkPipelineCacheCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			createInfo.pNext = VK_NULL_HANDLE;
			createInfo.flags = 0;
			createInfo.initialDataSize = buffer.size();
			createInfo.pInitialData = buffer.data();

			VkPipelineCache pipelineCache = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(getDevicePointerAs<VulkanDevice>()->getDeviceTable().vkCreatePipelineCache(getDevicePointerAs<VulkanDevice>()->getLogicalDevice(), &createInfo, nullptr, &pipelineCache), "Failed to create the pipeline cache!");

			return pipelineCache;
		}

		void VulkanRayTracingPipeline::saveCache(uint64_t identifier, VkPipelineCache cache) const
		{
			OPTICK_EVENT();

			// Return if we don't have anything to save.
			if (cache == VK_NULL_HANDLE)
				return;

			// Load cache data.
			size_t cacheSize = 0;
			FLINT_VK_ASSERT(getDevicePointerAs<VulkanDevice>()->getDeviceTable().vkGetPipelineCacheData(getDevicePointerAs<VulkanDevice>()->getLogicalDevice(), cache, &cacheSize, nullptr), "Failed to get the pipeline cache size!");

			auto buffer = std::vector<std::byte>(cacheSize);
			FLINT_VK_ASSERT(getDevicePointerAs<VulkanDevice>()->getDeviceTable().vkGetPipelineCacheData(getDevicePointerAs<VulkanDevice>()->getLogicalDevice(), cache, &cacheSize, buffer.data()), "Failed to get the pipeline cache data!");

			// Store the cache if possible.
			if (m_pCacheHandler)
				m_pCacheHandler->store(identifier, buffer);
		}

	}
}