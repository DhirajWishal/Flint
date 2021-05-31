// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanRenderTargetAttachment.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanSwapChain final : public VulkanRenderTargetAttachment 
		{
		public:
			VulkanSwapChain(VulkanDevice* pDevice, VulkanDisplay* pDisplay, const FExtent2D& extent, UI32 bufferCount);

			virtual void Recreate(const FExtent2D& extent) override final;
			virtual void Terminate() override final;

			virtual VkAttachmentDescription GetAttachmentDescription() const override final;
			virtual VkImageLayout GetAttachmentLayout() const override final;

			VkSwapchainKHR GetSwapChain() const { return vSwapChain; }

		private:
			VulkanDisplay* pDisplay = nullptr;
			VkSwapchainKHR vSwapChain = VK_NULL_HANDLE;
		};
	}
}