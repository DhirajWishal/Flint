// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Backend\FScreenBoundRenderTarget.h"
#include "VulkanRenderTarget.h"

#include "Attachments/VulkanColorBuffer.h"
#include "Attachments/VulkanSwapChain.h"
#include "Attachments/VulkanDepthBuffer.h"
#include "VulkanBackend\VulkanCommandBufferList.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanGraphicsPipeline;

		/**
		 * Software/ compute.
		 */
		class VulkanScreenBoundRenderTargetS final : public VulkanRenderTarget, public FScreenBoundRenderTarget {
		
		public:
			VulkanScreenBoundRenderTargetS(std::shared_ptr<FDevice> pDevice, std::shared_ptr<FDisplay> pDisplay, UI64 bufferCount);

			virtual void Terminate() override final;
			virtual void BakeCommands() override final;
			virtual void PrepareToDraw() override final;
			virtual void Update() override final;
			virtual void SubmitCommand() override final;

			virtual Vector2 VulkanRenderTarget::GetExtent() const = 0 { return FScreenBoundRenderTarget::GetExtent(); }

			VkSwapchainKHR GetSwapChain() const { return vSwapChain.GetSwapChain(); }
			VkFramebuffer GetCurrentFrameBuffer() const { return vFrameBuffers[GetFrameIndex()]; }

		private:
			void Recreate();

			void SubmitSecondaryCommands();

		private:
			VulkanSwapChain vSwapChain = {};
			VulkanColorBuffer vColorBuffer = {};
			VulkanDepthBuffer vDepthBuffer = {};
		};
	}
}