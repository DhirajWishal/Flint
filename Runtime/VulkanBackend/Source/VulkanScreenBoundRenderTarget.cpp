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
		VulkanScreenBoundRenderTarget::VulkanScreenBoundRenderTarget(VulkanDevice* m_pDevice, VulkanDisplay* pDisplay, const FBox2D& extent, const uint32_t bufferCount, const std::vector<RenderTargetAttachmentT>& imageAttachments, const SwapChainPresentMode presentMode, const FColor4D& swapChainClearColor)
			: ScreenBoundRenderTarget(m_pDevice, pDisplay, extent, bufferCount, imageAttachments, presentMode), m_vRenderTarget(*m_pDevice)
		{
			OPTICK_EVENT();

			pSwapChain = m_pDevice->CreateSwapChain(pDisplay, bufferCount, presentMode);

			m_vDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			m_vDependencies[0].dstSubpass = 0;
			m_vDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			m_vDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			m_vDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			m_vDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			m_vDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			m_vDependencies[1].srcSubpass = 0;
			m_vDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			m_vDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			m_vDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			m_vDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			m_vDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			m_vDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			std::vector<VulkanRenderTargetAttachmentInterface*> pAttachmentInferfaces;
			pAttachmentInferfaces.reserve(m_Attachments.size() + 1);

			bool bIsColorPresent = false;
			for (const auto attachment : m_Attachments)
			{
				pAttachmentInferfaces.emplace_back(attachment.pImage);

				VkClearValue m_vClearValue = {};
				if ((attachment.pImage->GetUsage() & ImageUsage::Color) == ImageUsage::Color)
				{
					m_vClearValue.color.float32[0] = attachment.m_ClearColor.m_Red;
					m_vClearValue.color.float32[1] = attachment.m_ClearColor.m_Green;
					m_vClearValue.color.float32[2] = attachment.m_ClearColor.m_Blue;
					m_vClearValue.color.float32[3] = attachment.m_ClearColor.m_Alpha;
					m_vClearAspectFlags.emplace_back(VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT);

					bIsColorPresent = true;
				}
				else if ((attachment.pImage->GetUsage() & ImageUsage::Depth) == ImageUsage::Depth)
				{
					m_vClearValue.depthStencil.depth = attachment.m_DepthClearValue.m_Depth;
					m_vClearValue.depthStencil.stencil = attachment.m_DepthClearValue.m_Stencil;
					m_vClearAspectFlags.emplace_back(VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT);
				}
				else
					throw backend_error("Invalid attachment type! The image usage should contain either Color or Depth.");

				m_vClearValues.emplace_back(m_vClearValue);
			}

			pAttachmentInferfaces.emplace_back(pSwapChain);
			if (!bIsColorPresent)
			{
				pSwapChain->ToggleClear();

				VkClearValue m_vClearValue = {};
				m_vClearValue.color.float32[0] = swapChainClearColor.m_Red;
				m_vClearValue.color.float32[1] = swapChainClearColor.m_Green;
				m_vClearValue.color.float32[2] = swapChainClearColor.m_Blue;
				m_vClearValue.color.float32[3] = swapChainClearColor.m_Alpha;

				m_vClearAspectFlags.emplace_back(VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT);
				m_vClearValues.emplace_back(m_vClearValue);
			}

			m_vRenderTarget.CreateRenderPass(pAttachmentInferfaces, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vDependencies);
			m_vRenderTarget.CreateFrameBuffer(pAttachmentInferfaces, extent, bufferCount);

			m_vInheritInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			m_vInheritInfo.pNext = VK_NULL_HANDLE;
			m_vInheritInfo.renderPass = m_vRenderTarget.m_vRenderPass;
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

			VkResult m_vResult = m_pDevice->GetDeviceTable().vkQueuePresentKHR(m_pDevice->GetQueue().m_vTransferQueue, pSwapChain->PrepareToPresent());
			if (m_vResult == VK_ERROR_OUT_OF_DATE_KHR || m_vResult == VK_SUBOPTIMAL_KHR)
				return false;
			else FLINT_VK_ASSERT(m_vResult);

			return true;
		}

		void VulkanScreenBoundRenderTarget::Terminate()
		{
			m_vRenderTarget.Terminate();
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

			m_pDevice->WaitIdle();

			m_vRenderTarget.DestroyRenderPass();
			m_vRenderTarget.DestroyFrameBuffers();

			std::vector<VulkanRenderTargetAttachmentInterface*> pAttachmentInferfaces;
			pAttachmentInferfaces.reserve(m_Attachments.size() + 1);

			for (auto attachment : m_Attachments)
			{
				attachment.pImage->Recreate(m_Extent);
				pAttachmentInferfaces.emplace_back(attachment.pImage);
			}

			pSwapChain->Recreate();
			pAttachmentInferfaces.emplace_back(pSwapChain.get());

			m_vRenderTarget.CreateRenderPass(pAttachmentInferfaces, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vDependencies);
			m_vRenderTarget.CreateFrameBuffer(pAttachmentInferfaces, m_Extent, m_BufferCount);

			m_vInheritInfo.renderPass = m_vRenderTarget.m_vRenderPass;
			m_FrameIndex = 0;

			bShouldRecreateResources = true;
		}

		const VkCommandBufferInheritanceInfo* VulkanScreenBoundRenderTarget::GetVulkanInheritanceInfo() const
		{
			m_vInheritInfo.framebuffer = GetFramebuffer();
			return &m_vInheritInfo;
		}
	}
}