// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanPipeline.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanGraphicsPipeline final : public VulkanPipeline {
		public:
			VulkanGraphicsPipeline() {}
			~VulkanGraphicsPipeline() {}

			void Initialize(VulkanDevice* pDevice, VulkanRenderTarget* pRenderTarget, const std::vector<ShaderDigest>& shaderDigest, const GraphicsPipelineSpecification& spec);
			void Terminate();

		private:
			GraphicsPipelineSpecification mSpec = {};

			std::vector<VkDescriptorPoolSize> vPoolSizes;
		};
	}
}
