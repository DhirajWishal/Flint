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

		class VulkanRenderTargetAttachmentInterface
		{
		public:
			VulkanRenderTargetAttachmentInterface() = default;

			virtual VkAttachmentDescription GetAttachmentDescription() const = 0;
			virtual VkImageLayout GetAttachmentLayout() const = 0;
			virtual VkImageView GetImageView(UI32 index) const = 0;
			virtual RenderTargetAttachmenType GetAttachmentType() const = 0;
			virtual VkFormat GetImageFormat() const = 0;
		};
	}
}