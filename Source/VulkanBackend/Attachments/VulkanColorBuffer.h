// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanRenderTargetAttachment.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanColorBuffer final : public VulkanRenderTargetAttachment 
		{
		public:
			VulkanColorBuffer(VulkanDevice* pDevice, const FExtent2D& extent, UI32 bufferCount, VkFormat format);

			virtual void Recreate(const FExtent2D& extent) override final;
			virtual void Terminate() override final;

			virtual VkAttachmentDescription GetAttachmentDescription() const override final;
			virtual VkImageLayout GetAttachmentLayout() const override final;

		private:
			VkDeviceMemory vBufferMemory = VK_NULL_HANDLE;
			VkSampleCountFlags vSampleCount = VK_SAMPLE_COUNT_64_BIT;
		};
	}
}