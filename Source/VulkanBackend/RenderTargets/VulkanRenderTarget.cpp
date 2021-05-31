// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "RenderTargets/VulkanRenderTarget.h"
#include "VulkanMacros.h"
#include "..\VulkanRenderTarget.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanRenderTarget::CreateRenderPass(VulkanDevice* pDevice, std::vector<VulkanRenderTargetAttachment*> pAttachments, VkPipelineBindPoint vBindPoint)
		{

		}

		void VulkanRenderTarget::DestroyRenderPass(VulkanDevice* pDevice)
		{
		}

		void VulkanRenderTarget::CreateFrameBuffer(VulkanDevice* pDevice, std::vector<VulkanRenderTargetAttachment*> pAttachments, const Vector2& extent, UI32 bufferCount)
		{

		}

		void VulkanRenderTarget::DestroyFrameBuffers(VulkanDevice* pDevice)
		{

		}

		void VulkanRenderTarget::InitializeSyncObjects(VulkanDevice* pDevice, UI32 count)
		{

		}

		void VulkanRenderTarget::TerminateSyncObjects(VulkanDevice* pDevice)
		{

		}
}
}