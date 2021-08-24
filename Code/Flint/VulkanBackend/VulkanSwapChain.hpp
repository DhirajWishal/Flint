// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanRenderTargetAttachment.hpp"
#include "VulkanDisplay.hpp"

#include "SwapChain.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanSwapChain final : public SwapChain, public VulkanRenderTargetAttachmentInterface
		{
		public:
			VulkanSwapChain(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, UI32 imageCount, SwapChainPresentMode presentMode);
			~VulkanSwapChain() { if (!bIsTerminated) Terminate(); }

			virtual void Recreate() override final;
			virtual NextImageInfo AcquireNextImage(UI32 frameIndex) override final;
			virtual void Terminate() override final;

			virtual RenderTargetAttachmenType GetAttachmentType() const override final { return RenderTargetAttachmenType::SWAP_CHAIN; }
			virtual VkFormat GetImageFormat() const override final;
			virtual VkAttachmentDescription GetAttachmentDescription() const override final;
			virtual VkImageLayout GetAttachmentLayout() const override final;

			const VkSwapchainKHR GetSwapChain() const { return vSwapChain; }

		private:
			void CreateSwapChain();
			void CreateSyncObjects();

		private:
			std::vector<VkImage> vImages = {};
			std::vector<VkImageView> vImageViews = {};

			std::vector<VkSemaphore> vPendingImages = {};
			std::vector<VkSemaphore> vPresentedImages = {};

			VkSwapchainKHR vSwapChain = VK_NULL_HANDLE;
		};
	}
}