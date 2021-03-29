// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Backend\ScreenBoundRenderTarget.h"
#include "VulkanRenderTarget.h"

#include "Attachments/VulkanColorBuffer.h"
#include "Attachments/VulkanSwapChain.h"
#include "Attachments/VulkanDepthBuffer.h"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Software/ compute.
		 */
		class VulkanScreenBoundRenderTargetS final : public VulkanRenderTarget, Backend::ScreenBoundRenderTarget<VulkanDevice, VulkanDisplay> {
		public:
			VulkanScreenBoundRenderTargetS() {}
			~VulkanScreenBoundRenderTargetS() {}

			virtual void Initialize(DeviceType* pDevice, DisplayType* pDisplay, UI64 bufferCount) override final;
			virtual void Terminate() override final;

			//Backend::GraphicsPipeline* CreateGraphicsPipeline(const std::vector<ShaderDigest>& shaderDigests, const Backend::GraphicsPipelineSpecification& spec);

			UI32 PrepareToDraw();
			void SubmitCommand();

			VkSwapchainKHR GetSwapChain() const { return vSwapChain.GetSwapChain(); }
			VkFramebuffer GetCurrentFrameBuffer() const { return vFrameBuffers[mImageIndex]; }

		private:
			void Recreate();

			void Bind(const std::shared_ptr<Backend::CommandBuffer>& pCommandBuffer);
			void UnBind(const std::shared_ptr<Backend::CommandBuffer>& pCommandBuffer);

			void SubmitSecondaryCommands();

		private:
			VulkanSwapChain vSwapChain = {};
			VulkanColorBuffer vColorBuffer = {};
			VulkanDepthBuffer vDepthBuffer = {};
		};
	}
}