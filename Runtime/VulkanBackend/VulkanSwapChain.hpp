// Copyright 2021-2022 Dhiraj Wishal
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
			VulkanSwapChain(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, uint32 imageCount, const SwapChainPresentMode presentMode);
			~VulkanSwapChain() { if (!bIsTerminated) Terminate(); }

			virtual void Recreate() override;
			virtual NextImageInfo AcquireNextImage(const uint32 frameIndex) override;
			virtual void Terminate() override;

			VkPresentInfoKHR* PrepareToPresent();

			virtual RenderTargetAttachmenType GetAttachmentType() const override final { return RenderTargetAttachmenType::SwapChain; }
			virtual VkFormat GetImageFormat() const override;
			virtual VkAttachmentDescription GetAttachmentDescription() const override;
			virtual VkImageLayout GetAttachmentLayout() const override;
			virtual VkImageView GetImageView(uint32 index) const override final { return vImageViews[index]; }

			const VkSwapchainKHR GetSwapChain() const { return vSwapChain; }
			const VkSwapchainKHR* GetSwapChainPtr() const { return &vSwapChain; }
			const uint32* GetImageIndexPtr() const { return &mImageIndex; }

			const VkSemaphore GetInFlightSemaphore() const { return vCurrentInFlightSemaphore; }
			const VkSemaphore GetRenderFinishedSemaphore() const { return vCurrentRenderFinishedSemaphore; }

			void ToggleClear() { bShouldClear = true; }

			void CopyFromImage(VkCommandBuffer vCommandBuffer, const VkImage vSrcImage, const VkImageLayout vSrcLayout, VkOffset3D srcOffset, VkOffset3D dstOffset, const uint32 index, const VkImageSubresourceLayers vSrcSubresourceLayer);

		private:
			void CreateSwapChain();
			void DestroySwapChain();

			void CreateSyncObjects();
			void DestroySyncObjects();

		private:
			std::vector<VkImage> vImages = {};
			std::vector<VkImageView> vImageViews = {};

			VkSwapchainKHR vSwapChain = VK_NULL_HANDLE;

			VkSemaphore vCurrentInFlightSemaphore = VK_NULL_HANDLE;
			VkSemaphore vCurrentRenderFinishedSemaphore = VK_NULL_HANDLE;

			std::vector< VkSemaphore> vInFlightSemaphores = {};
			std::vector< VkSemaphore> vRenderFinishedSemaphores = {};

			VkPresentInfoKHR vPresentInfo = {};
			bool bShouldClear = false;
		};
	}
}