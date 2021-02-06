// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Pipeline.h"
#include "VulkanBackend/RenderTargets/VulkanRenderTarget.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanPipeline {
		public:
			VulkanPipeline() {}
			virtual ~VulkanPipeline() {}

		protected:
			void ResolveUniformLayouts(const std::vector<ShaderDigest>& shaderDigests);

		protected:
			std::unordered_map<String, UniformLayout> mUniformLayouts;

			VkPipeline vPipeline = VK_NULL_HANDLE;
			VkPipelineLayout vPipelineLayout = VK_NULL_HANDLE;
			VkDescriptorSetLayout vSetLayout = VK_NULL_HANDLE;
		};
	}
}