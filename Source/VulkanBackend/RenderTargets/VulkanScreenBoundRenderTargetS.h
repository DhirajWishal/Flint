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
		class VulkanScreenBoundRenderTargetS final : public Backend::ScreenBoundRenderTarget, public VulkanRenderTarget {
		public:
			VulkanScreenBoundRenderTargetS() {}
			~VulkanScreenBoundRenderTargetS() {}

			virtual void Initialize(Backend::Device* pDevice, const Vector2& extent, UI32 bufferCount) override final;
			virtual void Terminate() override final;

			virtual Backend::GraphicsPipeline* CreateGraphicsPipeline(const std::vector<ShaderDigest>& shaderDigests, const Backend::GraphicsPipelineSpecification& spec) override final;

			VkSwapchainKHR GetSwapChain() const { return vSwapChain.GetSwapChain(); }

		private:
			virtual void Bind(Backend::CommandBuffer commandBuffer) override final;
			virtual void UnBind(Backend::CommandBuffer commandBuffer) override final;

		private:
			VulkanSwapChain vSwapChain = {};
			VulkanColorBuffer vColorBuffer = {};
			VulkanDepthBuffer vDepthBuffer = {};
		};
	}
}