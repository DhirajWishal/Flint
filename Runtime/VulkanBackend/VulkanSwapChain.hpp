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
		class VulkanSwapChain final : public SwapChain<VulkanDevice, VulkanDisplay>, public VulkanRenderTargetAttachmentInterface
		{
		public:
			explicit VulkanSwapChain(VulkanDevice* pDevice, VulkanDisplay* pDisplay, uint32_t imageCount, const SwapChainPresentMode presentMode);
			~VulkanSwapChain() { if (!bIsTerminated) Terminate(); }

			virtual void Recreate() override;
			virtual NextImageInfo AcquireNextImage(const uint32_t frameIndex) override;
			virtual void Terminate() override;

			VkPresentInfoKHR* PrepareToPresent();

			virtual RenderTargetAttachmenType GetAttachmentType() const override final { return RenderTargetAttachmenType::SwapChain; }
			virtual VkFormat GetImageFormat() const override;
			virtual VkAttachmentDescription GetAttachmentDescription() const override;
			virtual VkImageLayout GetAttachmentLayout() const override;
			virtual VkImageView GetImageView(uint32_t index) const override final { return m_vImageViews[index]; }

			const VkSwapchainKHR GetSwapChain() const { return m_vSwapChain; }
			const VkSwapchainKHR* GetSwapChainPtr() const { return &m_vSwapChain; }
			const uint32_t* GetImageIndexPtr() const { return &m_ImageIndex; }

			const VkSemaphore GetInFlightSemaphore() const { return m_vCurrentInFlightSemaphore; }
			const VkSemaphore GetRenderFinishedSemaphore() const { return m_vCurrentRenderFinishedSemaphore; }

			void ToggleClear() { bShouldClear = true; }

			void CopyFromImage(VkCommandBuffer m_vCommandBuffer, const VkImage m_vSrcImage, const VkImageLayout m_vSrcLayout, VkOffset3D srcOffset, VkOffset3D dstOffset, const uint32_t index, const VkImageSubresourceLayers m_vSrcSubresourceLayer);

		private:
			void CreateSwapChain();
			void DestroySwapChain();

			void CreateSyncObjects();
			void DestroySyncObjects();

		private:
			std::vector<VkImage> m_vImages = {};
			std::vector<VkImageView> m_vImageViews = {};

			VkSwapchainKHR m_vSwapChain = VK_NULL_HANDLE;

			VkSemaphore m_vCurrentInFlightSemaphore = VK_NULL_HANDLE;
			VkSemaphore m_vCurrentRenderFinishedSemaphore = VK_NULL_HANDLE;

			std::vector< VkSemaphore> m_vInFlightSemaphores = {};
			std::vector< VkSemaphore> m_vRenderFinishedSemaphores = {};

			VkPresentInfoKHR m_vPresentInfo = {};
			bool bShouldClear = false;
		};
	}
}