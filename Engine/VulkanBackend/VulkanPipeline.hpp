// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"

#include <filesystem>

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan pipeline class.
		 * This is the base class for the Vulkan pipelines.
		 */
		class VulkanPipeline
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The Vulkan engine.
			 * @param cacheFile The cache file name.
			 */
			explicit VulkanPipeline(const std::shared_ptr<VulkanDevice>& pDevice, const std::filesystem::path& cacheFile);

			/**
			 * Virtual destructor.
			 * Note that this will also destroy the pipeline, pipeline layout and the cache.
			 */
			virtual ~VulkanPipeline();

			/**
			 * Get the backend pipeline handle.
			 *
			 * @return The pipeline.
			 */
			[[nodiscard]] VkPipeline getPipeline() const noexcept { return m_Pipeline; }

			/**
			 * Get the pipeline layout.
			 *
			 * @return The pipeline layout.
			 */
			[[nodiscard]] VkPipelineLayout getPipelineLayout() const noexcept { return m_PipelineLayout; }

		private:
			/**
			 * Load the pipeline cache from the file.
			 */
			void loadCache();

		protected:
			/**
			 * Save the pipeline cache to the file.
			 */
			void saveCache();

		private:
			std::filesystem::path m_CacheFile;

			std::shared_ptr<VulkanDevice> m_pDevice = nullptr;

		protected:
			VkPipeline m_Pipeline = VK_NULL_HANDLE;
			VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
			VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
		};
	}
}