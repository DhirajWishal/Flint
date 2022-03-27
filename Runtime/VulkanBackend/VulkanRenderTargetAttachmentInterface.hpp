// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		enum class RenderTargetAttachmenType : uint8_t {
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
			virtual VkImageView GetImageView(uint32_t index) const = 0;
			virtual RenderTargetAttachmenType GetAttachmentType() const = 0;
			virtual VkFormat GetImageFormat() const = 0;
		};
	}
}