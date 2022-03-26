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

		class VulkanScreenBoundRenderTarget final : public ScreenBoundRenderTarget, public std::enable_shared_from_this<VulkanScreenBoundRenderTarget>
		{
		public:
			VulkanScreenBoundRenderTarget(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const uint32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments, const SwapChainPresentMode presentMode, const FColor4D& swapChainClearColor = FColor4D(0.0f));
			~VulkanScreenBoundRenderTarget() { if (!bIsTerminated) Terminate(); }

			virtual bool PrepareNewFrame() override;
			virtual bool PresentToDisplay() override;
			virtual void Terminate() override;
			virtual void Recreate() override;
			virtual const uint32 GetImageIndex() const override final { return pSwapChain->GetImageIndex(); }

			VkRenderPass GetRenderPass() const { return vRenderTarget.vRenderPass; }
			const VkFramebuffer GetFramebuffer() const { return vRenderTarget.vFrameBuffers[GetImageIndex()]; }

			const VulkanSwapChain* GetVulkanSwapChain() const { return static_cast<const VulkanSwapChain*>(pSwapChain.get()); }

			const uint32 GetClearScreenValueCount() const { return static_cast<uint32>(vClearValues.size()); }
			const VkClearValue* GetClearScreenValues() const { return vClearValues.data(); }

			std::vector<VkClearValue> GetClearScreenValueVector() const { return vClearValues; }
			std::vector<VkImageAspectFlags> GetClearAspectFlags() const { return vClearAspectFlags; }

			const VkCommandBufferInheritanceInfo* GetVulkanInheritanceInfo() const;

		private:
			VulkanRenderTarget vRenderTarget;

			std::vector<VkSubpassDependency> vDependencies{ 2 };
			std::atomic<VkCommandBufferInheritanceInfo> vInheritanceInfo = {};
			mutable VkCommandBufferInheritanceInfo vInheritInfo = {};

			std::vector<VkClearValue> vClearValues = {};
			std::vector<VkImageAspectFlags> vClearAspectFlags = {};

			bool bShouldSkip = false;
		};
	}
}