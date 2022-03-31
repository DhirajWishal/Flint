// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"
#include "VulkanRenderTargetAttachmentInterface.hpp"
#include "GraphicsCore/RenderTarget.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanRenderTarget
		{
		public:
			explicit VulkanRenderTarget(VulkanDevice& device);

			void Terminate();

			void CreateRenderPass(std::vector<VulkanRenderTargetAttachmentInterface*> pAttachments, VkPipelineBindPoint m_vBindPoint, const std::vector<VkSubpassDependency>& m_vSubpassDependencies);
			void CreateRenderPassWithMultipleSubpasses(std::vector<std::vector<VulkanRenderTargetAttachmentInterface*>> pSubpasses, VkPipelineBindPoint m_vBindPoint, const std::vector<VkSubpassDependency>& m_vSubpassDependencies);
			void DestroyRenderPass();

			void CreateFrameBuffer(std::vector<VulkanRenderTargetAttachmentInterface*> pAttachments, const FBox2D& extent, const uint32_t bufferCount);
			void DestroyFrameBuffers();

		public:
			VkFramebuffer CreateVulkanFrameBuffer(const FBox2D& extent, const std::vector<VkImageView>& m_vImageViews);

		public:
			std::vector<VkFramebuffer> m_vFrameBuffers = {};

			VkRenderPass m_vRenderPass = VK_NULL_HANDLE;

		private:
			VulkanDevice& m_vDevice;
		};
	}
}