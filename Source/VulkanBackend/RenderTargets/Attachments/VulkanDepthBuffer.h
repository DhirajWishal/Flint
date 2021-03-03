// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanRenderTargetAttachment.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDepthBuffer final : public VulkanRenderTargetAttachment {
		public:
			VulkanDepthBuffer() : VulkanRenderTargetAttachment(RenderTargetAttachmenType::DEPTH_BUFFER) {}

			virtual void Initialize(VulkanDevice* pDevice, const Vector2& extent, UI32 bufferCount) override final;
			virtual void Recreate(const Vector2& extent) override final;
			virtual void Terminate() override final;

			virtual VkAttachmentDescription GetAttachmentDescription() const override final;
			virtual VkImageLayout GetAttachmentLayout() const override final;

		private:
			VkDeviceMemory vBufferMemory = VK_NULL_HANDLE;
			VkSampleCountFlags vSampleCount = VK_SAMPLE_COUNT_64_BIT;
		};
	}
}