// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <unordered_map>

#include "Core\Backend\Pipeline.h"
#include "VulkanBackend\VulkanBuffer.h"
#include "VulkanBackend\VulkanImage.h"
#include "VulkanBackend\RenderTargets\VulkanRenderTarget.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanPipeline {
		public:
			VulkanPipeline() {}
			virtual ~VulkanPipeline() {}

			virtual VkPipelineBindPoint GetBindPoint() const = 0;
			virtual std::unordered_map<String, Backend::UniformLayout> GetUniformLayouts() const = 0;

			VkPipeline vPipeline = VK_NULL_HANDLE;
			VkPipelineLayout vPipelineLayout = VK_NULL_HANDLE;
			VkDescriptorSetLayout vSetLayout = VK_NULL_HANDLE;

			std::vector<VkDescriptorPoolSize> vPoolSizes;

		protected:
			VulkanRenderTarget* pvRenderTarget = nullptr;
		};
	}
}