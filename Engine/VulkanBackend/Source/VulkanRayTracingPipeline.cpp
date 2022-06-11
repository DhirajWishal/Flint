// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanRayTracingPipeline.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanRayTracingPipeline::VulkanRayTracingPipeline(VulkanDevice& device, std::filesystem::path&& cacheFile)
			: VulkanPipeline(device, cacheFile)
		{

		}

		VulkanRayTracingPipeline::~VulkanRayTracingPipeline()
		{

		}
	}
}