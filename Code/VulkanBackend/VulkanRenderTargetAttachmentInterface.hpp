// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		enum class RenderTargetAttachmenType : uint8 {
			Undefined,
			SwapChain,
			ColorBuffer,
			DepthBuffer,
		};

		class VulkanRenderTargetAttachmentInterface
		{
		public:
			VulkanRenderTargetAttachmentInterface() = default;

			virtual VkAttachmentDescription GetAttachmentDescription() const = 0;
			virtual VkImageLayout GetAttachmentLayout() const = 0;
			virtual VkImageView GetImageView(uint32 index) const = 0;
			virtual RenderTargetAttachmenType GetAttachmentType() const = 0;
			virtual VkFormat GetImageFormat() const = 0;
		};
	}
}