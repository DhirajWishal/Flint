// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanOffScreenRenderTarget.hpp"
#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "VulkanBackend/VulkanImage.hpp"

#include "GraphicsCore/GeometryStore.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanOffScreenRenderTarget::VulkanOffScreenRenderTarget(VulkanDevice* m_pDevice, const FBox2D& extent, const uint32_t bufferCount, const std::vector<typename RenderTarget<VulkanDevice, VulkanImage>::RenderTargetAttachmentT>& imageAttachments)
			: OffScreenRenderTarget(m_pDevice, extent, bufferCount, imageAttachments), m_vRenderTarget(*m_pDevice)
		{
			OPTICK_EVENT();

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
			pAttachmentInferfaces.reserve(m_Attachments.size());
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

			m_vRenderTarget.CreateRenderPass(pAttachmentInferfaces, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vDependencies);
			m_vRenderTarget.CreateFrameBuffer(pAttachmentInferfaces, extent, bufferCount);

			m_vInheritInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			m_vInheritInfo.pNext = VK_NULL_HANDLE;
			m_vInheritInfo.renderPass = m_vRenderTarget.m_vRenderPass;
		}
		
		void VulkanOffScreenRenderTarget::Recreate(const FBox2D& extent)
		{
			m_Extent = extent;

			m_pDevice->WaitIdle();

			m_vRenderTarget.DestroyRenderPass();
			m_vRenderTarget.DestroyFrameBuffers();

			std::vector<VulkanRenderTargetAttachmentInterface*> pAttachmentInferfaces;
			pAttachmentInferfaces.reserve(m_Attachments.size());

			for (auto attachment : m_Attachments)
			{
				attachment.pImage->Recreate(m_Extent);
				pAttachmentInferfaces.emplace_back(attachment.pImage);
			}

			m_vRenderTarget.CreateRenderPass(pAttachmentInferfaces, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vDependencies);
			m_vRenderTarget.CreateFrameBuffer(pAttachmentInferfaces, m_Extent, m_BufferCount);

			m_vInheritInfo.renderPass = m_vRenderTarget.m_vRenderPass;
			m_FrameIndex = 0;
		}

		bool VulkanOffScreenRenderTarget::PrepareNewFrame()
		{
			return true;
		}

		void VulkanOffScreenRenderTarget::Terminate()
		{
			m_vRenderTarget.Terminate();
			bIsTerminated = true;
		}
		
		const VkCommandBufferInheritanceInfo* VulkanOffScreenRenderTarget::GetVulkanInheritanceInfo() const
		{
			m_vInheritInfo.framebuffer = m_vRenderTarget.m_vFrameBuffers[m_FrameIndex];
			return &m_vInheritInfo;
		}
	}
}