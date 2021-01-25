// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/RenderTargets/VulkanRenderTargetSB3D.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanRenderTargetSB3D::Initialize(VulkanDevice* pDevice, const Interface::RenderTargetExtent& extent, UI32 bufferCount)
		{
			this->pDevice = pDevice;
			this->mExtent = extent;
			this->mBufferCount = pDevice->FindSupporterBufferCount(bufferCount);

			vSwapChain.Initialize(pDevice, extent, mBufferCount);
			vColorBuffer.Initialize(pDevice, extent, vSwapChain.GetFormat(), mBufferCount);
			vDepthBuffer.Initialize(pDevice, extent, mBufferCount);

			CreateRenderPass({ &vSwapChain, &vColorBuffer, &vDepthBuffer }, VK_PIPELINE_BIND_POINT_GRAPHICS);
			CreateFrameBuffer({ &vSwapChain, &vColorBuffer, &vDepthBuffer });
		}

		void VulkanRenderTargetSB3D::Terminate()
		{
			vSwapChain.Terminate();
			vColorBuffer.Terminate();
			vDepthBuffer.Terminate();

			DestroyRenderPass();
			DestroyFrameBuffers();
		}

		Interface::RenderTargetHandleSB3D CreateRenderTargetSB3D(const Interface::DeviceHandle& deviceHandle, const Interface::RenderTargetExtent& extent, UI32 bufferCount)
		{
			VulkanRenderTargetSB3D* pRenderTarget = new VulkanRenderTargetSB3D();
			pRenderTarget->Initialize(reinterpret_cast<VulkanDevice*>(HandleToPointer(deviceHandle)), extent, bufferCount);

			return PointerToHandle<Interface::RenderTargetHandleSB3D>(pRenderTarget);
		}

		void DestroyRenderTargetSB3D(const Interface::RenderTargetHandleSB3D& handle)
		{
			VulkanRenderTargetSB3D* pRenderTarget = reinterpret_cast<VulkanRenderTargetSB3D*>(HandleToPointer(handle));
			pRenderTarget->Terminate();

			delete pRenderTarget;
		}
	}
}