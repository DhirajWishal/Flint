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
#include "VulkanBackend\RenderTargets\VulkanScreenBoundRenderTarget.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanScreenBoundRenderTarget;

		class VulkanGraphicsPipeline final : public VulkanPipeline, public Backend::GraphicsPipeline
		{
		public:
			VulkanGraphicsPipeline(Backend::ScreenBoundRenderTarget* pRenderTarget, const std::vector<Backend::ShaderDigest>& shaderDigests, const Backend::GraphicsPipelineSpecification& spec);

			virtual void Terminate() override final;
			virtual void PrepareToRecreate() override final;
			virtual void Recreate() override final;

			virtual VkPipelineBindPoint GetBindPoint() const override final { return VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS; }

			virtual Backend::PipelineResource* CreatePipelineResource() override final;

			virtual std::unordered_map<String, Backend::UniformLayout> VulkanPipeline::GetUniformLayouts() const { return VulkanPipeline::GetUniformLayouts(); }

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
