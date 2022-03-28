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
		VulkanOffScreenRenderTarget::VulkanOffScreenRenderTarget(VulkanDevice* pDevice, const FBox2D& extent, const uint32_t bufferCount, const std::vector<typename RenderTarget<VulkanDevice, VulkanImage>::RenderTargetAttachmentT>& imageAttachments)
			: OffScreenRenderTarget(pDevice, extent, bufferCount, imageAttachments), vRenderTarget(*pDevice)
		{
			OPTICK_EVENT();

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
			pAttachmentInferfaces.reserve(m_Attachments.size());
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

			vRenderTarget.CreateRenderPass(pAttachmentInferfaces, VK_PIPELINE_BIND_POINT_GRAPHICS, vDependencies);
			vRenderTarget.CreateFrameBuffer(pAttachmentInferfaces, extent, bufferCount);

			vInheritInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			vInheritInfo.pNext = VK_NULL_HANDLE;
			vInheritInfo.renderPass = vRenderTarget.vRenderPass;
		}
		
		void VulkanOffScreenRenderTarget::Recreate(const FBox2D& extent)
		{
			m_Extent = extent;

			pDevice->WaitIdle();

			vRenderTarget.DestroyRenderPass();
			vRenderTarget.DestroyFrameBuffers();

			std::vector<VulkanRenderTargetAttachmentInterface*> pAttachmentInferfaces;
			pAttachmentInferfaces.reserve(m_Attachments.size());

			for (auto attachment : m_Attachments)
			{
				attachment.pImage->Recreate(m_Extent);
				pAttachmentInferfaces.emplace_back(attachment.pImage);
			}

			vRenderTarget.CreateRenderPass(pAttachmentInferfaces, VK_PIPELINE_BIND_POINT_GRAPHICS, vDependencies);
			vRenderTarget.CreateFrameBuffer(pAttachmentInferfaces, m_Extent, m_BufferCount);

			vInheritInfo.renderPass = vRenderTarget.vRenderPass;
			m_FrameIndex = 0;
		}

		bool VulkanOffScreenRenderTarget::PrepareNewFrame()
		{
			return true;
		}

		void VulkanOffScreenRenderTarget::Terminate()
		{
			vRenderTarget.Terminate();
			bIsTerminated = true;
		}
		
		const VkCommandBufferInheritanceInfo* VulkanOffScreenRenderTarget::GetVulkanInheritanceInfo() const
		{
			vInheritInfo.framebuffer = vRenderTarget.vFrameBuffers[m_FrameIndex];
			return &vInheritInfo;
		}
	}
}