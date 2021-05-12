// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/FRenderTarget.h"
#include "Attachments/VulkanRenderTargetAttachment.h"

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

			virtual Vector2 GetExtent() const = 0;

		protected:
			void CreateRenderPass(VulkanDevice* pDevice, std::vector<VulkanRenderTargetAttachment*> pAttachments, VkPipelineBindPoint vBindPoint);
			void DestroyRenderPass(VulkanDevice* pDevice);

			void CreateFrameBuffer(VulkanDevice* pDevice, std::vector<VulkanRenderTargetAttachment*> pAttachments, const Vector2& extent, UI32 bufferCount);
			void DestroyFrameBuffers(VulkanDevice* pDevice);

			void InitializeSyncObjects(VulkanDevice* pDevice, UI32 count);
			void TerminateSyncObjects(VulkanDevice* pDevice);

		protected:
			std::vector<VkSemaphore> vImageAvailables;
			std::vector<VkSemaphore> vRenderFinishes;
			std::vector<VkFence> vInFlightFences;
			std::vector<VkFence> vImagesInFlightFences;

			std::vector<VkFramebuffer> vFrameBuffers;
			VkRenderPass vRenderPass = VK_NULL_HANDLE;
		};
	}
}