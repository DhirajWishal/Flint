// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/RenderTargets/VulkanRenderTarget.h"
#include "VulkanBackend/VulkanMacros.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanRenderTarget::CreateRenderPass(std::vector<VulkanRenderTargetAttachment*> pAttachments, VkPipelineBindPoint vBindPoint)
		{
			std::vector<VkAttachmentDescription> vDescriptions;

			std::vector<VkAttachmentReference> vColorAttachmentRef;
			std::vector<VkAttachmentReference> vDepthAttachmentRef;
			std::vector<VkAttachmentReference> vResolveAttachmentRef;

			VkAttachmentReference vAR = {};
			vAR.attachment = 0;

			VkSubpassDescription vSD = {};
			vSD.flags = VK_NULL_HANDLE;
			vSD.colorAttachmentCount = 0;
			vSD.inputAttachmentCount = 0;
			vSD.preserveAttachmentCount = 0;
			vSD.pipelineBindPoint = vBindPoint;

			for (auto itr = pAttachments.begin(); itr != pAttachments.end(); itr++)
			{
				INSERT_INTO_VECTOR(vDescriptions, (*itr)->GetAttachmentDescription());

				vAR.layout = (*itr)->GetAttachmentLayout();

				switch ((*itr)->GetType())
				{
				case Flint::VulkanBackend::RenderTargetAttachmenType::SWAP_CHAIN:
					INSERT_INTO_VECTOR(vResolveAttachmentRef, vAR);
					vSD.colorAttachmentCount++;
					break;

				case Flint::VulkanBackend::RenderTargetAttachmenType::COLOR_BUFFER:
					INSERT_INTO_VECTOR(vColorAttachmentRef, vAR);
					break;

				case Flint::VulkanBackend::RenderTargetAttachmenType::DEPTH_BUFFER:
					INSERT_INTO_VECTOR(vDepthAttachmentRef, vAR);
					break;

				default:
					FLINT_LOG_ERROR(TEXT("Invalid or Undefined render target attachment type!"))
						break;
				}

				vAR.attachment++;
			}

			vSD.pColorAttachments = vColorAttachmentRef.data();
			vSD.pDepthStencilAttachment = vDepthAttachmentRef.data();
			vSD.pResolveAttachments = vResolveAttachmentRef.data();

			VkSubpassDependency vSDep = {};
			vSDep.srcSubpass = VK_SUBPASS_EXTERNAL;
			vSDep.dstSubpass = 0;
			vSDep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			vSDep.srcAccessMask = 0;
			vSDep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			vSDep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			VkRenderPassCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			vCI.flags = VK_NULL_HANDLE;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.attachmentCount = static_cast<UI32>(vDescriptions.size());
			vCI.pAttachments = vDescriptions.data();
			vCI.subpassCount = 1;
			vCI.pSubpasses = &vSD;
			vCI.dependencyCount = 1;
			vCI.pDependencies = &vSDep;

			FLINT_VK_ASSERT(pDevice->CreateRenderPass(&vCI, &vRenderPass), "Failed to create render pass!");
		}

		void VulkanRenderTarget::DestroyRenderPass()
		{
			pDevice->DestroyRenderPass(vRenderPass);
		}

		void VulkanRenderTarget::CreateFrameBuffer(std::vector<VulkanRenderTargetAttachment*> pAttachments)
		{
			VkFramebufferCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			vCI.flags = VK_NULL_HANDLE;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.layers = 1;
			vCI.renderPass = vRenderPass;
			vCI.width = mExtent.mWidth;
			vCI.height = mExtent.mHeight;
			vCI.attachmentCount = static_cast<UI32>(pAttachments.size());

			vFrameBuffers.resize(mBufferCount);
			for (UI32 i = 0; i < mBufferCount; i++)
			{
				std::vector<VkImageView> vAttachments;

				for (auto itr = pAttachments.begin(); itr != pAttachments.end(); itr++)
					INSERT_INTO_VECTOR(vAttachments, (*itr)->GetImageView(i));

				vCI.pAttachments = vAttachments.data();
				FLINT_VK_ASSERT(pDevice->CreateFrameBuffer(&vCI, vFrameBuffers.data() + i), "Failed to create frame buffer!");
			}
		}

		void VulkanRenderTarget::DestroyFrameBuffers()
		{
			pDevice->DestroyFrameBuffers(vFrameBuffers);
			vFrameBuffers.clear();
		}
	}
}