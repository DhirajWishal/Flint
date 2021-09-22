// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanRenderTargetAttachmentInterface.hpp"
#include "VulkanDisplay.hpp"

#include "GraphicsCore/SwapChain.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanSwapChain final : public SwapChain, public VulkanRenderTargetAttachmentInterface
		{
		public:
			VulkanSwapChain(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, UI32 imageCount, const SwapChainPresentMode presentMode);
			~VulkanSwapChain() { if (!bIsTerminated) Terminate(); }

			virtual void Recreate() override final;
			virtual NextImageInfo AcquireNextImage() override final;
			virtual void Terminate() override final;

			VkPresentInfoKHR* PrepareToPresent();

			virtual RenderTargetAttachmenType GetAttachmentType() const override final { return RenderTargetAttachmenType::SwapChain; }
			virtual VkFormat GetImageFormat() const override final;
			virtual VkAttachmentDescription GetAttachmentDescription() const override final;
			virtual VkImageLayout GetAttachmentLayout() const override final;
			virtual VkImageView GetImageView(UI32 index) const override final { return vImageViews[index]; }

			const VkSwapchainKHR GetSwapChain() const { return vSwapChain; }
			const VkSwapchainKHR* GetSwapChainPtr() const { return &vSwapChain; }
			const UI32* GetImageIndexPtr() const { return &mImageIndex; }

			const VkSemaphore GetInFlightSemaphore() const { return vInFlightSemaphore; }
			const VkSemaphore GetRenderFinishedSemaphore() const { return vRenderFinished; }

		private:
			void CreateSwapChain();
			void DestroySwapChain();

			void CreateSyncObjects();
			void DestroySyncObjects();

		private:
			std::vector<VkImage> vImages = {};
			std::vector<VkImageView> vImageViews = {};

			VkSwapchainKHR vSwapChain = VK_NULL_HANDLE;

			VkSemaphore vInFlightSemaphore = VK_NULL_HANDLE;
			VkSemaphore vRenderFinished = VK_NULL_HANDLE;

			VkPresentInfoKHR vPresentInfo = {};
		};
	}
}