// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanPipeline.hpp"
#include "VulkanDescriptorSetManager.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan ray tracing pipeline.
		 */
		class VulkanRayTracingPipeline final : public VulkanPipeline
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device to which the pipeline is bound to.
			 * @param cacheFile The cache file to save the pipeline cache to.
			 */
			explicit VulkanRayTracingPipeline(VulkanDevice& device, std::filesystem::path&& cacheFile);

			/**
			 * Destructor.
			 */
			~VulkanRayTracingPipeline() override;
		};
	}
}