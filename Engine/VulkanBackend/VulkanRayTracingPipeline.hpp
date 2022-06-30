// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/RayTracingPipeline.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanDescriptorSetManager.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan ray tracing pipeline.
		 */
		class VulkanRayTracingPipeline final : public RayTracingPipeline, public VulkanPipeline
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device to which the pipeline is bound to.
			 * @param pCacheHandler The pipeline cache handler used to handle the pipeline cache. Default is nullptr.
			 */
			explicit VulkanRayTracingPipeline(const std::shared_ptr<VulkanDevice>& pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler = nullptr);

			/**
			 * Destructor.
			 */
			~VulkanRayTracingPipeline() override;

			/**
			 * Terminate the object.
			 */
			void terminate() override;

			/**
			 * Load the pipeline cache from the handler if possible.
			 */
			void loadCache() override;

			/**
			 * Save the pipeline cache from the handler if possible.
			 */
			void saveCache() override;
		};
	}
}