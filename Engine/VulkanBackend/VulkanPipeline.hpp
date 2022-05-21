// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanEngine.hpp"

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
			 * @param engine The Vulkan engine.
			 * @param name The name of the pipeline. This will be used to load and save the pipeline cache.
			 */
			explicit VulkanPipeline(VulkanEngine& engine, std::string&& name);

			/**
			 * Virtual destructor.
			 * Note that this will also destroy the pipeline, pipeline layout and the cache.
			 */
			virtual ~VulkanPipeline();

			/**
			 * Get the engine.
			 *
			 * @return The Vulkan engine.
			 */
			[[nodiscard]] VulkanEngine& getEngine() { return m_Engine; }

			/**
			 * Get the engine.
			 *
			 * @return The Vulkan engine.
			 */
			[[nodiscard]] const VulkanEngine& getEngine() const { return m_Engine; }

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

		protected:
			std::filesystem::path m_CacheFile;

			VulkanEngine& m_Engine;

			VkPipeline m_Pipeline = VK_NULL_HANDLE;
			VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
			VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
		};
	}
}