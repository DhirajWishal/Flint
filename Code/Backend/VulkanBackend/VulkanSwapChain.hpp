// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanRenderTargetAttachment.hpp"
#include "VulkanDisplay.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanSwapChain final : public VulkanRenderTargetAttachment
		{
		public:
			VulkanSwapChain(VulkanDevice& device, VulkanDisplay& display, const FExtent2D& extent, const UI32 bufferCount);

			virtual void Recreate(const FExtent2D& extent) override final;
			virtual void Terminate() override final;

			virtual VkAttachmentDescription GetAttachmentDescription() const override final;
			virtual VkImageLayout GetAttachmentLayout() const override final;

			VkSwapchainKHR GetSwapChain() const { return vSwapChain; }

		private:
			void Initialize();

		private:
			VulkanDisplay& vDisplay;
			VkSwapchainKHR vSwapChain = VK_NULL_HANDLE;
		};
	}
}