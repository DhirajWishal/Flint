// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanRenderTargetAttachment.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanSwapChain final : public VulkanRenderTargetAttachment {
		public:
			VulkanSwapChain() : VulkanRenderTargetAttachment(RenderTargetAttachmenType::SWAP_CHAIN) {}

			virtual void Initialize(VulkanDevice* pDevice, const Vector2& extent, UI32 bufferCount) override final;
			virtual void Recreate(const Vector2& extent) override final;
			virtual void Terminate() override final;

			virtual VkAttachmentDescription GetAttachmentDescription() const override final;
			virtual VkImageLayout GetAttachmentLayout() const override final;

			VkSwapchainKHR GetSwapChain() const { return vSwapChain; }

		private:
			VkSwapchainKHR vSwapChain = VK_NULL_HANDLE;
		};
	}
}