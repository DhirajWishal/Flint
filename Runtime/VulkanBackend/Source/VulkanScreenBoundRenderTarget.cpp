// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "VulkanBackend/VulkanImage.hpp"
#include "GraphicsCore/GeometryStore.hpp"

#include <iostream>

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanScreenBoundRenderTarget::VulkanScreenBoundRenderTarget(VulkanDevice* pDevice, VulkanDisplay* pDisplay, const FBox2D& extent, const uint32_t bufferCount, const std::vector<RenderTargetAttachmentT>& imageAttachments, const SwapChainPresentMode presentMode, const FColor4D& swapChainClearColor)
			: ScreenBoundRenderTarget(pDevice, pDisplay, extent, bufferCount, imageAttachments, presentMode), vRenderTarget(*pDevice)
		{
			OPTICK_EVENT();

			pSwapChain = pDevice->CreateSwapChain(pDisplay, bufferCount, presentMode);

			vDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			vDependencies[0].dstSubpass = 0;
			vDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			vDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			vDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			vDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			vDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			vDependencies[1].srcSubpass = 0;
			vDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			vDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			vDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			vDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			vDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			vDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			std::vector<VulkanRenderTargetAttachmentInterface*> pAttachmentInferfaces;
			pAttachmentInferfaces.reserve(m_Attachments.size() + 1);

			bool bIsColorPresent = false;
			for (const auto attachment : m_Attachments)
			{
				pAttachmentInferfaces.emplace_back(attachment.pImage);

				VkClearValue vClearValue = {};
				if ((attachment.pImage->GetUsage() & ImageUsage::Color) == ImageUsage::Color)
				{
					vClearValue.color.float32[0] = attachment.m_ClearColor.m_Red;
					vClearValue.color.float32[1] = attachment.m_ClearColor.m_Green;
					vClearValue.color.float32[2] = attachment.m_ClearColor.m_Blue;
					vClearValue.color.float32[3] = attachment.m_ClearColor.m_Alpha;
					vClearAspectFlags.emplace_back(VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT);

					bIsColorPresent = true;
				}
				else if ((attachment.pImage->GetUsage() & ImageUsage::Depth) == ImageUsage::Depth)
				{
					vClearValue.depthStencil.depth = attachment.m_DepthClearValue.m_Depth;
					vClearValue.depthStencil.stencil = attachment.m_DepthClearValue.m_Stencil;
					vClearAspectFlags.emplace_back(VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT);
				}
				else
					throw backend_error("Invalid attachment type! The image usage should contain either Color or Depth.");

				vClearValues.emplace_back(vClearValue);
			}

			pAttachmentInferfaces.emplace_back(pSwapChain);
			if (!bIsColorPresent)
			{
				pSwapChain->ToggleClear();

				VkClearValue vClearValue = {};
				vClearValue.color.float32[0] = swapChainClearColor.m_Red;
				vClearValue.color.float32[1] = swapChainClearColor.m_Green;
				vClearValue.color.float32[2] = swapChainClearColor.m_Blue;
				vClearValue.color.float32[3] = swapChainClearColor.m_Alpha;

				vClearAspectFlags.emplace_back(VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT);
				vClearValues.emplace_back(vClearValue);
			}

			vRenderTarget.CreateRenderPass(pAttachmentInferfaces, VK_PIPELINE_BIND_POINT_GRAPHICS, vDependencies);
			vRenderTarget.CreateFrameBuffer(pAttachmentInferfaces, extent, bufferCount);

			vInheritInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			vInheritInfo.pNext = VK_NULL_HANDLE;
			vInheritInfo.renderPass = vRenderTarget.vRenderPass;
		}

		bool VulkanScreenBoundRenderTarget::PrepareNewFrame()
		{
			OPTICK_EVENT();

			auto nextImage = pSwapChain->AcquireNextImage(m_FrameIndex);
			return !nextImage.bShouldRecreate;
		}

		bool VulkanScreenBoundRenderTarget::PresentToDisplay()
		{
			OPTICK_EVENT();

			VkResult vResult = pDevice->GetDeviceTable().vkQueuePresentKHR(pDevice->GetQueue().vTransferQueue, pSwapChain->PrepareToPresent());
			if (vResult == VK_ERROR_OUT_OF_DATE_KHR || vResult == VK_SUBOPTIMAL_KHR)
				return false;
			else FLINT_VK_ASSERT(vResult);

			return true;
		}

		void VulkanScreenBoundRenderTarget::Terminate()
		{
			vRenderTarget.Terminate();
			pSwapChain->Terminate();

			bIsTerminated = true;
		}

		void VulkanScreenBoundRenderTarget::Recreate()
		{
			OPTICK_EVENT();

			FBox2D newExtent = pDisplay->GetExtent();
			if (newExtent.IsZero())
			{
				bShouldSkip = true;
				return;
			}

			m_Extent = newExtent;
			bShouldSkip = false;

			pDevice->WaitIdle();

			vRenderTarget.DestroyRenderPass();
			vRenderTarget.DestroyFrameBuffers();

			std::vector<VulkanRenderTargetAttachmentInterface*> pAttachmentInferfaces;
			pAttachmentInferfaces.reserve(m_Attachments.size() + 1);

			for (auto attachment : m_Attachments)
			{
				attachment.pImage->Recreate(m_Extent);
				pAttachmentInferfaces.emplace_back(attachment.pImage);
			}

			pSwapChain->Recreate();
			pAttachmentInferfaces.emplace_back(pSwapChain.get());

			vRenderTarget.CreateRenderPass(pAttachmentInferfaces, VK_PIPELINE_BIND_POINT_GRAPHICS, vDependencies);
			vRenderTarget.CreateFrameBuffer(pAttachmentInferfaces, m_Extent, m_BufferCount);

			vInheritInfo.renderPass = vRenderTarget.vRenderPass;
			m_FrameIndex = 0;

			bShouldRecreateResources = true;
		}

		const VkCommandBufferInheritanceInfo* VulkanScreenBoundRenderTarget::GetVulkanInheritanceInfo() const
		{
			vInheritInfo.framebuffer = GetFramebuffer();
			return &vInheritInfo;
		}
	}
}