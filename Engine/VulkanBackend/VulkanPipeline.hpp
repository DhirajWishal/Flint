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
			 */
			explicit VulkanPipeline(const std::shared_ptr<VulkanDevice>& pDevice);

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

		private:
			std::shared_ptr<VulkanDevice> m_pDevice = nullptr;

		protected:
			VkPipeline m_Pipeline = VK_NULL_HANDLE;
			VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
		};
	}
}