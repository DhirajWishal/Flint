// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/ScreenBoundRenderTarget.hpp"
#include "VulkanRenderTarget.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanColorBuffer.hpp"
#include "VulkanDepthBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanScreenBoundRenderTarget final : public ScreenBoundRenderTarget
		{
		public:
			VulkanScreenBoundRenderTarget(Device& device, Display& display, const FExtent2D& extent, const UI32 bufferCount);

			virtual void BeginFrame() override final;
			virtual void Update() override final;
			virtual void SubmitFrame() override final;
			virtual void Terminate() override final;

			virtual void Recreate() override final;

		private:
			VulkanRenderTarget vRenderTarget;

			VulkanSwapChain* pSwapChain = nullptr;
			VulkanColorBuffer* pColorBuffer = nullptr;
			VulkanDepthBuffer* pDepthBuffer = nullptr;
		};
	}
}