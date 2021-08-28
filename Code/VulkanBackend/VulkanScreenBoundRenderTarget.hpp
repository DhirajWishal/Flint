// Copyright 2021 Dhiraj Wishal
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
			VulkanScreenBoundRenderTarget(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const UI32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments, const SwapChainPresentMode presentMode);
			~VulkanScreenBoundRenderTarget() { if (!bIsTerminated) Terminate(); }

			virtual void PrepareNewFrame() override final;
			virtual void PresentToDisplay() override final;
			virtual void Terminate() override final;
			virtual void Recreate() override final;
			virtual const UI32 GetImageIndex() const override final { return pSwapChain->GetImageIndex(); }

			VkRenderPass GetRenderPass() const { return vRenderTarget.vRenderPass; }
			const VkFramebuffer GetFramebuffer() const { return vRenderTarget.vFrameBuffers[GetImageIndex()]; }

			const VulkanSwapChain* GetSwapChain() const { return pSwapChain.get(); }

			const UI32 GetClearScreenValueCount() const { return static_cast<UI32>(mAttachments.size()); }
			const VkClearValue* GetClearScreenValues() const { return vClearValues.data(); }

			const VkCommandBufferInheritanceInfo* GetVulkanInheritanceInfo();

		private:
			VulkanRenderTarget vRenderTarget;

			std::vector<VkSubpassDependency> vDependencies{ 2 };
			std::atomic<VkCommandBufferInheritanceInfo> vInheritanceInfo = {};
			VkCommandBufferInheritanceInfo vInheritInfo = {};

			std::vector<VkClearValue> vClearValues = {};

			std::unique_ptr<VulkanSwapChain> pSwapChain = nullptr;

			bool bShouldSkip = false;
		};
	}
}