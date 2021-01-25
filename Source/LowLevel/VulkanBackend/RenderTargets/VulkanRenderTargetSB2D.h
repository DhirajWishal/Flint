// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanRenderTarget.h"

#include "Attachments/VulkanColorBuffer.h"
#include "Attachments/VulkanSwapChain.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanRenderTargetSB2D final : public VulkanRenderTarget {
		public:
			VulkanRenderTargetSB2D() {}
			~VulkanRenderTargetSB2D() {}

			virtual void Initialize(VulkanDevice* pDevice, const Interface::RenderTargetExtent& extent, UI32 bufferCount) override final;
			virtual void Terminate() override final;

		private:
			VulkanSwapChain vSwapChain = {};
			VulkanColorBuffer vColorBuffer = {};
		};

		Interface::RenderTargetHandleSB2D CreateRenderTargetSB2D(const Interface::DeviceHandle& deviceHandle, const Interface::RenderTargetExtent& extent, UI32 bufferCount);
		void DestroyRenderTargetSB2D(const Interface::RenderTargetHandleSB2D& handle);
	}
}