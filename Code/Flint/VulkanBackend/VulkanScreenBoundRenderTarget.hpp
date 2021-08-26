// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ScreenBoundRenderTarget.hpp"
#include "Thread/CommandCenter.hpp"

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

			virtual void PresentToDisplay() override final;
			virtual void Terminate() override final;
			virtual void Recreate() override final;

			VkRenderPass GetRenderPass() const { return vRenderTarget.vRenderPass; }
			VkFramebuffer GetFrameBuffer(UI32 index) const { return vRenderTarget.vFrameBuffers[index]; }
			const VkFramebuffer PrepareAndGetFramebuffer();

			const VulkanSwapChain* GetSwapChain() const { return pSwapChain.get(); }

			UI32 GetClearScreenValueCount() const { return static_cast<UI32>(mAttachments.size()); }
			std::vector<VkClearValue> GetClearScreenValues() const;

			const VkCommandBufferInheritanceInfo* GetVulkanInheritanceInfo() const { return &vInheritInfo; }

		private:
			VulkanRenderTarget vRenderTarget;

			std::vector<VkSubpassDependency> vDependencies{ 2 };
			std::atomic<VkCommandBufferInheritanceInfo> vInheritanceInfo = {};
			VkCommandBufferInheritanceInfo vInheritInfo = {};

			std::unique_ptr<VulkanSwapChain> pSwapChain = nullptr;

			bool bShouldSkip = false;
		};
	}
}

/*
typedef struct VkSubmitInfo {
	VkStructureType                sType;
	const void*                    pNext;
	uint32_t                       waitSemaphoreCount;
	const VkSemaphore*             pWaitSemaphores;
	const VkPipelineStageFlags*    pWaitDstStageMask;
	uint32_t                       commandBufferCount;
	const VkCommandBuffer*         pCommandBuffers;
	uint32_t                       signalSemaphoreCount;
	const VkSemaphore*             pSignalSemaphores;
} VkSubmitInfo;

typedef struct VkPresentInfoKHR {
	VkStructureType          sType;
	const void*              pNext;
	uint32_t                 waitSemaphoreCount;
	const VkSemaphore*       pWaitSemaphores;
	uint32_t                 swapchainCount;
	const VkSwapchainKHR*    pSwapchains;
	const uint32_t*          pImageIndices;
	VkResult*                pResults;
} VkPresentInfoKHR;
*/