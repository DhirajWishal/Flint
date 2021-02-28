// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/RenderTargets/VulkanRenderTarget.h"
#include "VulkanBackend/VulkanMacros.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanRenderTarget::CreateRenderPass(VulkanDevice* pDevice, std::vector<VulkanRenderTargetAttachment*> pAttachments, VkPipelineBindPoint vBindPoint)
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
					//INSERT_INTO_VECTOR(vResolveAttachmentRef, vAR);
					//vSD.colorAttachmentCount++;
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

			VkSubpassDependency dependencies[2] = {};

			dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[0].dstSubpass = 0;
			dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			dependencies[1].srcSubpass = 0;
			dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			VkRenderPassCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			vCI.flags = VK_NULL_HANDLE;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.attachmentCount = static_cast<UI32>(vDescriptions.size());
			vCI.pAttachments = vDescriptions.data();
			vCI.subpassCount = 1;
			vCI.pSubpasses = &vSD;
			vCI.dependencyCount = 2;
			vCI.pDependencies = dependencies;

			FLINT_VK_ASSERT(pDevice->CreateRenderPass(&vCI, &vRenderPass), "Failed to create render pass!");
		}

		void VulkanRenderTarget::DestroyRenderPass(VulkanDevice* pDevice)
		{
			pDevice->DestroyRenderPass(vRenderPass);
		}

		void VulkanRenderTarget::CreateFrameBuffer(VulkanDevice* pDevice, std::vector<VulkanRenderTargetAttachment*> pAttachments, const Vector2& extent, UI32 bufferCount)
		{
			VkFramebufferCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			vCI.flags = VK_NULL_HANDLE;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.layers = 1;
			vCI.renderPass = vRenderPass;
			vCI.width = static_cast<UI32>(extent.x);
			vCI.height = static_cast<UI32>(extent.y);
			vCI.attachmentCount = static_cast<UI32>(pAttachments.size());

			vFrameBuffers.resize(bufferCount);
			for (UI32 i = 0; i < bufferCount; i++)
			{
				std::vector<VkImageView> vAttachments;

				for (auto itr = pAttachments.begin(); itr != pAttachments.end(); itr++)
					INSERT_INTO_VECTOR(vAttachments, (*itr)->GetImageView(i));

				vCI.pAttachments = vAttachments.data();
				FLINT_VK_ASSERT(pDevice->CreateFrameBuffer(&vCI, vFrameBuffers.data() + i), "Failed to create frame buffer!");
			}
		}

		void VulkanRenderTarget::DestroyFrameBuffers(VulkanDevice* pDevice)
		{
			pDevice->DestroyFrameBuffers(vFrameBuffers);
			vFrameBuffers.clear();
		}

		void VulkanRenderTarget::InitializeSyncObjects(VulkanDevice* pDevice, UI32 count)
		{
			vImageAvailables.resize(count);
			vRenderFinishes.resize(count);
			vInFlightFences.resize(count);

			VkSemaphoreCreateInfo vSCI = {};
			vSCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo VFCI = {};
			VFCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			VFCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();
			FLINT_VK_ASSERT(pvDevice->CreateSemaphores(&vSCI, vImageAvailables), "Failed to create Image Available semaphores!")
				FLINT_VK_ASSERT(pvDevice->CreateSemaphores(&vSCI, vRenderFinishes), "Failed to create Render Finished semaphores!")
				FLINT_VK_ASSERT(pvDevice->CreateFences(&VFCI, vInFlightFences), "Failed to create In Flight fences!")
		}

		void VulkanRenderTarget::TerminateSyncObjects(VulkanDevice* pDevice)
		{
			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();
			pvDevice->DestroySemaphores(vImageAvailables);
			pvDevice->DestroySemaphores(vRenderFinishes);
			pvDevice->DestroyFences(vInFlightFences);
		}
	}
}