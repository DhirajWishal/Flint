// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Attachments/VulkanRenderTargetAttachment.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanRenderTarget : public VulkanDeviceBoundObject {
		public:
			VulkanRenderTarget() {}

			virtual void Initialize(VulkanDevice* pDevice, const Interface::RenderTargetExtent& extent, UI32 bufferCount) {}

		public:
			VkRenderPass GetRenderPass() const { return vRenderPass; }

		protected:
			void CreateRenderPass(std::vector<VulkanRenderTargetAttachment*> pAttachments, VkPipelineBindPoint vBindPoint);
			void DestroyRenderPass();

			void CreateFrameBuffer(std::vector<VulkanRenderTargetAttachment*> pAttachments);
			void DestroyFrameBuffers();

		protected:
			std::vector<VkFramebuffer> vFrameBuffers;
			VkRenderPass vRenderPass = VK_NULL_HANDLE;

			Interface::RenderTargetExtent mExtent = {};
			UI32 mBufferCount = 0;
		};
	}
}