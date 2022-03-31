// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/ScreenBoundRenderTarget.hpp"

#include "VulkanRenderTarget.hpp"
#include "VulkanSwapChain.hpp"

#include "Core/BinarySemaphore.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBufferList;

		class VulkanScreenBoundRenderTarget final : public ScreenBoundRenderTarget<VulkanDevice, VulkanImage, VulkanDisplay, VulkanSwapChain>
		{
		public:
			using RenderTargetAttachmentT = typename RenderTarget<VulkanDevice, VulkanImage>::RenderTargetAttachmentT;

			explicit VulkanScreenBoundRenderTarget(VulkanDevice* pDevice, VulkanDisplay* pDisplay, const FBox2D& extent, const uint32_t bufferCount, const std::vector<RenderTargetAttachmentT>& imageAttachments, const SwapChainPresentMode presentMode, const FColor4D& swapChainClearColor = FColor4D(0.0f));
			~VulkanScreenBoundRenderTarget() { if (!bIsTerminated) Terminate(); }

			virtual bool PrepareNewFrame() override;
			virtual bool PresentToDisplay() override;
			virtual void Terminate() override;
			virtual void Recreate() override;
			virtual const uint32_t GetImageIndex() const override final { return pSwapChain->GetImageIndex(); }

			VkRenderPass GetRenderPass() const { return m_vRenderTarget.m_vRenderPass; }
			const VkFramebuffer GetFramebuffer() const { return m_vRenderTarget.m_vFrameBuffers[GetImageIndex()]; }

			const VulkanSwapChain* GetVulkanSwapChain() const { return static_cast<const VulkanSwapChain*>(pSwapChain.get()); }

			const uint32_t GetClearScreenValueCount() const { return static_cast<uint32_t>(m_vClearValues.size()); }
			const VkClearValue* GetClearScreenValues() const { return m_vClearValues.data(); }

			std::vector<VkClearValue> GetClearScreenValueVector() const { return m_vClearValues; }
			std::vector<VkImageAspectFlags> GetClearAspectFlags() const { return m_vClearAspectFlags; }

			const VkCommandBufferInheritanceInfo* GetVulkanInheritanceInfo() const;

		private:
			VulkanRenderTarget m_vRenderTarget;

			std::vector<VkSubpassDependency> m_vDependencies{ 2 };
			std::atomic<VkCommandBufferInheritanceInfo> m_vInheritanceInfo = {};
			mutable VkCommandBufferInheritanceInfo m_vInheritInfo = {};

			std::vector<VkClearValue> m_vClearValues = {};
			std::vector<VkImageAspectFlags> m_vClearAspectFlags = {};

			bool bShouldSkip = false;
		};
	}
}