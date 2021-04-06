// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanPipeline.h"
#include "Core\Backend\GraphicsPipeline.h"
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

		class VulkanGraphicsPipeline final : public VulkanPipeline, public Backend::GraphicsPipeline<VulkanGraphicsPipeline, VulkanDevice, VulkanRenderTarget, VulkanBuffer, VulkanImage, VulkanPipelineResource> {
		public:
			using Super = Backend::GraphicsPipeline<VulkanGraphicsPipeline, VulkanDevice, VulkanRenderTarget, VulkanBuffer, VulkanImage, VulkanPipelineResource>;
			using DeviceType = VulkanDevice;
			using BufferType = VulkanBuffer;
			using ResourceType = VulkanPipelineResource;

		public:
			VulkanGraphicsPipeline() {}
			~VulkanGraphicsPipeline() {}

			virtual void Initialize(VulkanRenderTarget* pRenderTarget, const std::vector<ShaderDigest>& shaderDigests, const Backend::GraphicsPipelineSpecification& spec) override final;
			virtual void PrepareToRecreate() override final;
			virtual void Recreate() override final { RecreatePipeline(); }
			virtual void Terminate() override final;

			virtual VkPipelineBindPoint GetBindPoint() const override final { return VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS; }

			virtual VulkanPipelineResource CreatePipelineResource() override final;

			virtual VulkanRenderTarget* VulkanPipeline::GetRenderTarget() const { return Super::GetRenderTarget(); }
			virtual std::unordered_map<String, UniformLayout> VulkanPipeline::GetUniformLayouts() const { return Super::GetUniformLayouts(); }

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
