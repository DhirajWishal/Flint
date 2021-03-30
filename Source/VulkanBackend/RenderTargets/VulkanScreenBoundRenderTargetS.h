// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Backend\ScreenBoundRenderTarget.h"
#include "VulkanRenderTarget.h"

#include "Attachments/VulkanColorBuffer.h"
#include "Attachments/VulkanSwapChain.h"
#include "Attachments/VulkanDepthBuffer.h"
#include "VulkanBackend\VulkanCommandBufferList.h"

namespace Flint
{
	namespace VulkanBackend
	{
		template<class TRenderTarget>
		class VulkanGraphicsPipeline;

		/**
		 * Software/ compute.
		 */
		class VulkanScreenBoundRenderTargetS final : public VulkanRenderTarget, public Backend::ScreenBoundRenderTarget<VulkanDevice, VulkanDisplay, VulkanCommandBufferList, VulkanGraphicsPipeline<VulkanScreenBoundRenderTargetS>> {
		public:
			using DeviceType = VulkanDevice;
			using DisplayType = VulkanDisplay;
		
		public:
			VulkanScreenBoundRenderTargetS() {}
			~VulkanScreenBoundRenderTargetS() {}

			virtual void Initialize(DeviceType* pDevice, DisplayType* pDisplay, UI64 bufferCount) override final;
			virtual void Terminate() override final;

			virtual void BakeCommands() override final;
			virtual void PrepareToDraw() override final;
			virtual void Update() override final;
			virtual void SubmitCommand() override final;

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