// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/RenderTarget.h"
#include "Attachments/VulkanRenderTargetAttachment.h"
#include "VulkanBackend\VulkanCommandBufferManager.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanRenderTarget {
		public:
			VulkanRenderTarget() {}

		public:
			VkRenderPass GetRenderPass() const { return vRenderPass; }
			virtual VkFramebuffer GetCurrentFrameBuffer() const = 0;

		protected:
			void CreateRenderPass(std::shared_ptr<VulkanDevice> pDevice, std::vector<VulkanRenderTargetAttachment*> pAttachments, VkPipelineBindPoint vBindPoint);
			void DestroyRenderPass(std::shared_ptr<VulkanDevice> pDevice);

			void CreateFrameBuffer(std::shared_ptr<VulkanDevice> pDevice, std::vector<VulkanRenderTargetAttachment*> pAttachments, const Vector2& extent, UI32 bufferCount);
			void DestroyFrameBuffers(std::shared_ptr<VulkanDevice> pDevice);

			void InitializeSyncObjects(std::shared_ptr<VulkanDevice> pDevice, UI32 count);
			void TerminateSyncObjects(std::shared_ptr<VulkanDevice> pDevice);

		protected:
			VulkanCommandBufferManager mCommandBufferManager = {};

			std::vector<VkSemaphore> vImageAvailables;
			std::vector<VkSemaphore> vRenderFinishes;
			std::vector<VkFence> vInFlightFences;
			std::vector<VkFence> vImagesInFlightFences;

			std::vector<VkFramebuffer> vFrameBuffers;
			VkRenderPass vRenderPass = VK_NULL_HANDLE;
		};
	}
}