// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanRenderTargetAttachmentInterface.hpp"
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
			virtual NextImageInfo AcquireNextImage() override final;
			virtual void Terminate() override final;

			VkPresentInfoKHR* PrepareToPresent();

			virtual RenderTargetAttachmenType GetAttachmentType() const override final { return RenderTargetAttachmenType::SWAP_CHAIN; }
			virtual VkFormat GetImageFormat() const override final;
			virtual VkAttachmentDescription GetAttachmentDescription() const override final;
			virtual VkImageLayout GetAttachmentLayout() const override final;
			virtual VkImageView GetImageView(UI32 index) const override final { return vImageViews[index]; }

			const VkSwapchainKHR GetSwapChain() const { return vSwapChain; }
			const VkSwapchainKHR* GetSwapChainPtr() const { return &vSwapChain; }
			const UI32* GetImageIndexPtr() const { return &mImageIndex; }

		private:
			void CreateSwapChain();
			void CreateSyncObjects();

		private:
			std::vector<VkImage> vImages = {};
			std::vector<VkImageView> vImageViews = {};

			VkSwapchainKHR vSwapChain = VK_NULL_HANDLE;
			VkSemaphore vSemaphore = VK_NULL_HANDLE;

			VkPresentInfoKHR vPresentInfo = {};
		};
	}
}