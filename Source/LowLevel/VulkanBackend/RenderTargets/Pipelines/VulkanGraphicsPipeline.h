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

		Interface::GraphicsPipelineHandle CreateGraphicsPipelineSB2D(const Interface::RenderTargetHandleSB2D& renderTargetHandle, const std::vector<ShaderDigest>& digests, const GraphicsPipelineSpecification& spec);
		Interface::GraphicsPipelineHandle CreateGraphicsPipelineSB3D(const Interface::RenderTargetHandleSB3D& renderTargetHandle, const std::vector<ShaderDigest>& digests, const GraphicsPipelineSpecification& spec);
		Interface::GraphicsPipelineHandle CreateGraphicsPipelineSBRT(const Interface::RenderTargetHandleSBRT& renderTargetHandle, const std::vector<ShaderDigest>& digests, const GraphicsPipelineSpecification& spec);

		Interface::GraphicsPipelineHandle CreateGraphicsPipelineOS2D(const Interface::RenderTargetHandleOS2D& renderTargetHandle, const std::vector<ShaderDigest>& digests, const GraphicsPipelineSpecification& spec);
		Interface::GraphicsPipelineHandle CreateGraphicsPipelineOS3D(const Interface::RenderTargetHandleOS3D& renderTargetHandle, const std::vector<ShaderDigest>& digests, const GraphicsPipelineSpecification& spec);
		Interface::GraphicsPipelineHandle CreateGraphicsPipelineOSRT(const Interface::RenderTargetHandleOSRT& renderTargetHandle, const std::vector<ShaderDigest>& digests, const GraphicsPipelineSpecification& spec);

		void DestroyGraphicsPipeline(const Interface::GraphicsPipelineHandle& handle);
	}
}
