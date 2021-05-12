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
		class VulkanGraphicsPipeline;

		class VulkanScreenBoundRenderTarget final : public VulkanRenderTarget, public Backend::ScreenBoundRenderTarget
		{
		public:
			VulkanScreenBoundRenderTarget(Backend::Device* pDevice, Backend::Display* pDisplay, UI64 bufferCount);

			virtual void Terminate() override final;
			virtual void BakeCommands() override final;
			virtual void PrepareToDraw() override final;
			virtual void Update() override final;
			virtual void SubmitCommand() override final;

			virtual Vector2 VulkanRenderTarget::GetExtent() const = 0 { return ScreenBoundRenderTarget::GetExtent(); }

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