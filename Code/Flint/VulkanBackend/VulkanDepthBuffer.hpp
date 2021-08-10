// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanRenderTargetAttachment.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDepthBuffer final : public VulkanRenderTargetAttachment
		{
		public:
			VulkanDepthBuffer(VulkanDevice& device, const FBox2D& extent, const UI32 bufferCount);

			virtual void Recreate(const FBox2D& extent) override final;
			void Terminate();

			virtual VkAttachmentDescription GetAttachmentDescription() const override final;
			virtual VkImageLayout GetAttachmentLayout() const override final;

		private:
			void Initialize();

		private:
			VkDeviceMemory vBufferMemory = VK_NULL_HANDLE;
			VkSampleCountFlags vSampleCount = VK_SAMPLE_COUNT_64_BIT;
		};
	}
}