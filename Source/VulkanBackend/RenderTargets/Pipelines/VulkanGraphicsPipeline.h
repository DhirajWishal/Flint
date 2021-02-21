// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanPipeline.h"
#include "Core/Backend/GraphicsPipeline.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanGraphicsPipeline final : public VulkanPipeline, public Backend::GraphicsPipeline {
		public:
			VulkanGraphicsPipeline() {}
			~VulkanGraphicsPipeline() {}

			virtual void Initialize(Backend::RenderTarget* pRenderTarget, const std::vector<ShaderDigest>& shaderDigests, const Backend::GraphicsPipelineSpecification& spec) override final;
			virtual void Terminate() override final;

			virtual void Bind(const Backend::CommandBuffer& commandBuffer) override final;
			virtual void UnBind(const Backend::CommandBuffer& commandBuffer) override final;

		private:
			std::vector<VkDescriptorPoolSize> vPoolSizes;
		};
	}
}
