// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/RenderTarget.h"
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

		protected:
			void CreateRenderPass(VulkanDevice* pDevice, std::vector<VulkanRenderTargetAttachment*> pAttachments, VkPipelineBindPoint vBindPoint);
			void DestroyRenderPass(VulkanDevice* pDevice);

			void CreateFrameBuffer(VulkanDevice* pDevice, std::vector<VulkanRenderTargetAttachment*> pAttachments, const Vector2& extent);
			void DestroyFrameBuffers(VulkanDevice* pDevice);

		protected:
			std::vector<VkFramebuffer> vFrameBuffers;
			VkRenderPass vRenderPass = VK_NULL_HANDLE;

			UI32 mBufferCount = 0;
		};
	}
}