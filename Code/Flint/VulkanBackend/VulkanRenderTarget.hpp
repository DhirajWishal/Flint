// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"
#include "VulkanRenderTargetAttachment.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanRenderTarget
		{
		public:
			VulkanRenderTarget(VulkanDevice& device);

			void Terminate();

			void CreateRenderPass(std::vector<VulkanRenderTargetAttachmentInterface*> pAttachments, VkPipelineBindPoint vBindPoint, const std::vector<VkSubpassDependency>& vSubpassDependencies);
			void CreateRenderPassWithMultipleSubpasses(std::vector<std::vector<VulkanRenderTargetAttachmentInterface*>> pSubpasses, VkPipelineBindPoint vBindPoint, const std::vector<VkSubpassDependency>& vSubpassDependencies);
			void DestroyRenderPass();

			void CreateFrameBuffer(std::vector<VulkanRenderTargetAttachmentInterface*> pAttachments, const FBox2D& extent, UI32 bufferCount);
			void DestroyFrameBuffers();

			void CreateSyncObjects(UI32 count);
			void DestroySyncObjects();

		public:
			VkFramebuffer CreateVulkanFrameBuffer(const FBox2D& extent, const std::vector<VkImageView>& vImageViews);

		public:
			std::vector<VkSemaphore> vImageAvailables;
			std::vector<VkSemaphore> vRenderFinishes;
			std::vector<VkFence> vInFlightFences;
			std::vector<VkFence> vImagesInFlightFences;
			std::vector<VkFramebuffer> vFrameBuffers = {};

			VkRenderPass vRenderPass = VK_NULL_HANDLE;

		private:
			VulkanDevice& vDevice;
		};
	}
}