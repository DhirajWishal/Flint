// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		enum class RenderTargetAttachmenType : UI8 {
			UNDEFINED,
			SWAP_CHAIN,
			COLOR_BUFFER,
			DEPTH_BUFFER,
		};

		class VulkanRenderTargetAttachment
		{
		public:
			VulkanRenderTargetAttachment(RenderTargetAttachmenType type, VulkanDevice& device, const FExtent2D& extent, const UI32 bufferCount, VkFormat format)
				: mType(type), vDevice(device), mExtent(extent), mBufferCount(bufferCount), vFormat(format) {}

			VulkanRenderTargetAttachment(RenderTargetAttachmenType type, VulkanDevice& device, const FExtent2D& extent, const UI32 bufferCount)
				: mType(type), vDevice(device), mExtent(extent), mBufferCount(bufferCount) {}

			virtual void Recreate(const FExtent2D& extent) = 0;
			virtual void Terminate() = 0;

			virtual VkAttachmentDescription GetAttachmentDescription() const = 0;
			virtual VkImageLayout GetAttachmentLayout() const = 0;

			VkImageView GetImageView(UI32 index) const { return vImageViews[index]; }

		public:
			RenderTargetAttachmenType GetType() const { return mType; }
			VkFormat GetFormat() const { return vFormat; }

		protected:
			std::vector<VkImage> vImages;
			std::vector<VkImageView> vImageViews;
			VulkanDevice& vDevice;
			FExtent2D mExtent = {};
			UI32 mBufferCount = 0;
			VkFormat vFormat = VkFormat::VK_FORMAT_UNDEFINED;
			RenderTargetAttachmenType mType = RenderTargetAttachmenType::UNDEFINED;
		};
	}
}
