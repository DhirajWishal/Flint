// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

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
		class VulkanScreenBoundRenderTargetS final : public VulkanRenderTarget {
		public:
			VulkanScreenBoundRenderTargetS() {}
			~VulkanScreenBoundRenderTargetS() {}

			void Initialize(VulkanDevice* pDevice, VulkanDisplay* pDisplay, const Vector2& extent, UI32 bufferCount);
			virtual void Terminate() override final;

			Backend::GraphicsPipeline* CreateGraphicsPipeline(const std::vector<ShaderDigest>& shaderDigests, const Backend::GraphicsPipelineSpecification& spec);

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
			VulkanDisplay* pDisplay = nullptr;
			VulkanSwapChain vSwapChain = {};
			VulkanColorBuffer vColorBuffer = {};
			VulkanDepthBuffer vDepthBuffer = {};
		};
	}
}