// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ScreenBoundRenderTarget.hpp"
#include "VulkanRenderTarget.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanColorBuffer.hpp"
#include "VulkanDepthBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanScreenBoundRenderTarget final : public ScreenBoundRenderTarget, public std::enable_shared_from_this<VulkanScreenBoundRenderTarget>
		{
		public:
			VulkanScreenBoundRenderTarget(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, const FExtent2D& extent, const UI32 bufferCount);

			virtual void PrepareStaticResources() override final;
			virtual void BeginFrame() override final;
			virtual void Update() override final;
			virtual void SubmitFrame() override final;
			virtual void Terminate() override final;

			virtual void Recreate() override final;

			VkRenderPass GetRenderPass() const { return vRenderTarget.vRenderPass; }
			VkFramebuffer GetFrameBuffer(UI32 index) const { return vRenderTarget.vFrameBuffers[index]; }

		private:
			VulkanRenderTarget vRenderTarget;

			VulkanSwapChain* pSwapChain = nullptr;
			VulkanColorBuffer* pColorBuffer = nullptr;
			VulkanDepthBuffer* pDepthBuffer = nullptr;

			bool bShouldSkip = false;
		};
	}
}