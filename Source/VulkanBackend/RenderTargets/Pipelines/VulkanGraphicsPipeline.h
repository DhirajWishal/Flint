// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanPipeline.h"
#include "Core\Backend\FGraphicsPipeline.h"
#include "VulkanBackend\VulkanShaderModule.h"
#include "VulkanBackend\VulkanUtilities.h"
#include "VulkanBackend\VulkanMacros.h"
#include "VulkanBackend\VulkanBuffer.h"
#include "VulkanBackend\VulkanImage.h"
#include "VulkanBackend\RenderTargets\VulkanScreenBoundRenderTargetS.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanScreenBoundRenderTargetS;

		class VulkanGraphicsPipeline final : public VulkanPipeline, public FGraphicsPipeline {
		public:
			VulkanGraphicsPipeline(std::shared_ptr<FRenderTarget> pRenderTarget, const std::vector<ShaderDigest>& shaderDigests, const GraphicsPipelineSpecification& spec);
			~VulkanGraphicsPipeline();

			virtual void PrepareToRecreate() override final;
			virtual void Recreate() override final { RecreatePipeline(); }

			virtual VkPipelineBindPoint GetBindPoint() const override final { return VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS; }

			virtual std::shared_ptr<FPipelineResource> CreatePipelineResource() override final;

			virtual VulkanRenderTarget* VulkanPipeline::GetRenderTarget() const { return VulkanPipeline::GetRenderTarget(); }
			virtual std::unordered_map<String, UniformLayout> VulkanPipeline::GetUniformLayouts() const { return VulkanPipeline::GetUniformLayouts(); }

		private:
			virtual void RecreatePipeline() override final;

		private:
			VkPipelineInputAssemblyStateCreateInfo vIASCI = {};
			VkPipelineRasterizationStateCreateInfo vRSCI = {};
			VkPipelineViewportStateCreateInfo vVSCI = {};
			VkPipelineMultisampleStateCreateInfo vMSCI = {};

			VkPipelineColorBlendAttachmentState vCBAS = {};
			VkPipelineColorBlendStateCreateInfo vCBSCI = {};

			VkPipelineDepthStencilStateCreateInfo vDSSCI = {};
		};
	}
}
