// Copyright 2021 Dhiraj Wishal
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
			VulkanRenderTarget(VulkanDevice& device);

			void Terminate();

			void CreateRenderPass(boost::container::vector<VulkanRenderTargetAttachmentInterface*> pAttachments, VkPipelineBindPoint vBindPoint, const boost::container::vector<VkSubpassDependency>& vSubpassDependencies);
			void CreateRenderPassWithMultipleSubpasses(boost::container::vector<boost::container::vector<VulkanRenderTargetAttachmentInterface*>> pSubpasses, VkPipelineBindPoint vBindPoint, const boost::container::vector<VkSubpassDependency>& vSubpassDependencies);
			void DestroyRenderPass();

			void CreateFrameBuffer(boost::container::vector<VulkanRenderTargetAttachmentInterface*> pAttachments, const FBox2D& extent, const UI32 bufferCount);
			void DestroyFrameBuffers();

		public:
			VkFramebuffer CreateVulkanFrameBuffer(const FBox2D& extent, const boost::container::vector<VkImageView>& vImageViews);

		public:
			boost::container::vector<VkFramebuffer> vFrameBuffers = {};

			VkRenderPass vRenderPass = VK_NULL_HANDLE;

		private:
			VulkanDevice& vDevice;
		};
	}
}