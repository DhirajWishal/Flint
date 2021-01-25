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
		class VulkanRenderTargetSB3D final : public VulkanRenderTarget {
		public:
			VulkanRenderTargetSB3D() {}
			~VulkanRenderTargetSB3D() {}

			virtual void Initialize(VulkanDevice* pDevice, const Interface::RenderTargetExtent& extent, UI32 bufferCount) override final;
			virtual void Terminate() override final;

		private:
			VulkanSwapChain vSwapChain = {};
			VulkanColorBuffer vColorBuffer = {};
			VulkanDepthBuffer vDepthBuffer = {};
		};

		Interface::RenderTargetHandleSB3D CreateRenderTargetSB3D(const Interface::DeviceHandle& deviceHandle, const Interface::RenderTargetExtent& extent, UI32 bufferCount);
		void DestroyRenderTargetSB3D(const Interface::RenderTargetHandleSB3D& handle);
	}
}