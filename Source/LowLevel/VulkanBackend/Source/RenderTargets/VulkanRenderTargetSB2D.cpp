// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/RenderTargets/VulkanRenderTargetSB2D.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanRenderTargetSB2D::Initialize(VulkanDevice* pDevice, const Interface::RenderTargetExtent& extent, UI32 bufferCount)
		{
			this->pDevice = pDevice;
			this->mExtent = extent;
			this->mBufferCount = pDevice->FindSupporterBufferCount(bufferCount);

			vSwapChain.Initialize(pDevice, extent, mBufferCount);
			vColorBuffer.Initialize(pDevice, extent, vSwapChain.GetFormat(), mBufferCount);

			CreateRenderPass({ &vSwapChain, &vColorBuffer }, VK_PIPELINE_BIND_POINT_GRAPHICS);
			CreateFrameBuffer({ &vSwapChain, &vColorBuffer });
		}

		void VulkanRenderTargetSB2D::Terminate()
		{
			vSwapChain.Terminate();
			vColorBuffer.Terminate();

			DestroyRenderPass();
			DestroyFrameBuffers();
		}

		Interface::RenderTargetHandleSB2D CreateRenderTargetSB2D(const Interface::DeviceHandle& deviceHandle, const Interface::RenderTargetExtent& extent, UI32 bufferCount)
		{
			VulkanRenderTargetSB2D* pRenderTarget = new VulkanRenderTargetSB2D();
			pRenderTarget->Initialize(static_cast<VulkanDevice*>(HandleToPointer(deviceHandle)), extent, bufferCount);

			return PointerToHandle<Interface::RenderTargetHandleSB2D>(pRenderTarget);
		}

		void DestroyRenderTargetSB2D(const Interface::RenderTargetHandleSB2D& handle)
		{
			VulkanRenderTargetSB2D* pRenderTarget = static_cast<VulkanRenderTargetSB2D*>(HandleToPointer(handle));
			pRenderTarget->Terminate();

			delete pRenderTarget;
		}
	}
}