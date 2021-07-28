// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanGraphicsPipeline.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanGraphicsPipeline::VulkanGraphicsPipeline(
			const std::shared_ptr<Device>& pDevice,
			const std::shared_ptr<RenderTarget>& pRenderTarget,
			const std::shared_ptr<Shader>& pVertexShader,
			const std::shared_ptr<Shader>& pTessellationControlShader,
			const std::shared_ptr<Shader>& pTessellationEvaluationShader,
			const std::shared_ptr<Shader>& pGeometryShader,
			const std::shared_ptr<Shader>& pFragmentShader)
			: GraphicsPipeline(pDevice, pRenderTarget, pVertexShader, pTessellationControlShader, pTessellationEvaluationShader, pGeometryShader, pFragmentShader)
		{
			FLINT_SETUP_PROFILER();

			CreatePipelineLayout();
			CreatePipeline();
		}

		void VulkanGraphicsPipeline::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vkDestroyPipeline(vDevice.GetLogicalDevice(), vPipeline, nullptr);
			vkDestroyPipelineLayout(vDevice.GetLogicalDevice(), vPipelineLayout, nullptr);
		}

		void VulkanGraphicsPipeline::CreatePipelineLayout()
		{
			FLINT_SETUP_PROFILER();

			VkPipelineLayoutCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.flags = 0;

			FLINT_VK_ASSERT(vkCreatePipelineLayout(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), &vCI, nullptr, &vPipelineLayout));
		}

		void VulkanGraphicsPipeline::CreatePipeline()
		{
			FLINT_SETUP_PROFILER();

			VkGraphicsPipelineCreateInfo vCI = {};

			FLINT_VK_ASSERT(vkCreateGraphicsPipelines(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), VK_NULL_HANDLE, 1, &vCI, nullptr, &vPipeline));
		}
	}
}