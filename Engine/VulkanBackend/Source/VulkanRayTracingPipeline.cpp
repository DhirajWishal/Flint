// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanRayTracingPipeline.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanRayTracingPipeline::VulkanRayTracingPipeline(const std::shared_ptr<VulkanDevice>& pDevice, std::filesystem::path&& cacheFile)
			: RayTracingPipeline(pDevice)
			, VulkanPipeline(pDevice, cacheFile)
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
	}
}