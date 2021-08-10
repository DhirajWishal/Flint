// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanRenderTarget.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanRenderTarget::VulkanRenderTarget(VulkanDevice& device)
			: vDevice(device)
		{
		}

		void VulkanRenderTarget::Terminate()
		{
			DestroyRenderPass();
			DestroyFrameBuffers();
			DestroySyncObjects();
		}

		void VulkanRenderTarget::CreateRenderPass(std::vector<VulkanRenderTargetAttachmentInterface*> pAttachments, VkPipelineBindPoint vBindPoint, const std::vector<VkSubpassDependency>& vSubpassDependencies)
		{
			FLINT_SETUP_PROFILER();

			std::vector<VkAttachmentDescription> vDescriptions;

			std::vector<VkAttachmentReference> vColorAttachmentRef;
			std::vector<VkAttachmentReference> vDepthAttachmentRef;
			std::vector<VkAttachmentReference> vResolveAttachmentRef;

			VkAttachmentReference vAR = {};
			vAR.attachment = 0;

			VkSubpassDescription vSD = {};
			vSD.flags = 0;
			vSD.colorAttachmentCount = 0;
			vSD.inputAttachmentCount = 0;
			vSD.preserveAttachmentCount = 0;
			vSD.pipelineBindPoint = vBindPoint;

			for (auto itr = pAttachments.begin(); itr != pAttachments.end(); itr++)
			{
				if (!(*itr))
					continue;

				INSERT_INTO_VECTOR(vDescriptions, (*itr)->GetAttachmentDescription());

				vAR.layout = (*itr)->GetAttachmentLayout();

				switch ((*itr)->GetAttachmentType())
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
					FLINT_THROW_RUNTIME_ERROR("Invalid or Undefined render target attachment type!");
				}

				vAR.attachment++;
			}

			vSD.pColorAttachments = vColorAttachmentRef.data();
			vSD.pDepthStencilAttachment = vDepthAttachmentRef.data();
			vSD.pResolveAttachments = vResolveAttachmentRef.data();

			VkRenderPassCreateInfo vCI = {};
			vCI.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			vCI.flags = 0;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.attachmentCount = static_cast<UI32>(vDescriptions.size());
			vCI.pAttachments = vDescriptions.data();
			vCI.subpassCount = 1;
			vCI.pSubpasses = &vSD;
			vCI.dependencyCount = static_cast<UI32>(vSubpassDependencies.size());
			vCI.pDependencies = vSubpassDependencies.data();

			FLINT_VK_ASSERT(vkCreateRenderPass(vDevice.GetLogicalDevice(), &vCI, nullptr, &vRenderPass));
		}

		void VulkanRenderTarget::DestroyRenderPass()
		{
			vkDestroyRenderPass(vDevice.GetLogicalDevice(), vRenderPass, nullptr);
		}

		void VulkanRenderTarget::CreateFrameBuffer(std::vector<VulkanRenderTargetAttachmentInterface*> pAttachments, const FBox2D& extent, UI32 bufferCount)
		{
			FLINT_SETUP_PROFILER();

			VkFramebufferCreateInfo vCI = {};
			vCI.sType = VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			vCI.flags = 0;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.layers = 1;
			vCI.renderPass = vRenderPass;
			vCI.width = static_cast<UI32>(extent.mWidth);
			vCI.height = static_cast<UI32>(extent.mHeight);
			vCI.attachmentCount = static_cast<UI32>(pAttachments.size());

			vFrameBuffers.resize(bufferCount);
			for (UI32 i = 0; i < bufferCount; i++)
			{
				std::vector<VkImageView> vAttachments;

				for (auto itr = pAttachments.begin(); itr != pAttachments.end(); itr++)
					INSERT_INTO_VECTOR(vAttachments, (*itr)->GetImageView(i));

				vCI.pAttachments = vAttachments.data();
				FLINT_VK_ASSERT(vkCreateFramebuffer(vDevice.GetLogicalDevice(), &vCI, nullptr, &vFrameBuffers[i]));
			}
		}

		void VulkanRenderTarget::DestroyFrameBuffers()
		{
			for (auto itr : vFrameBuffers)
				vkDestroyFramebuffer(vDevice.GetLogicalDevice(), itr, nullptr);

			vFrameBuffers.clear();
		}

		void VulkanRenderTarget::CreateSyncObjects(UI32 count)
		{
			FLINT_SETUP_PROFILER();

			vImageAvailables.resize(count);
			vRenderFinishes.resize(count);
			vInFlightFences.resize(count);
			vImagesInFlightFences.resize(count);

			VkSemaphoreCreateInfo vSCI = {};
			vSCI.sType = VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo vFCI = {};
			vFCI.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			vFCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for (auto& itr : vImageAvailables)
				FLINT_VK_ASSERT(vkCreateSemaphore(vDevice.GetLogicalDevice(), &vSCI, nullptr, &itr));

			for (auto& itr : vRenderFinishes)
				FLINT_VK_ASSERT(vkCreateSemaphore(vDevice.GetLogicalDevice(), &vSCI, nullptr, &itr));

			for (auto& itr : vInFlightFences)
				vkCreateFence(vDevice.GetLogicalDevice(), &vFCI, nullptr, &itr);
		}

		void VulkanRenderTarget::DestroySyncObjects()
		{
			for (auto& itr : vImageAvailables)
				vkDestroySemaphore(vDevice.GetLogicalDevice(), itr, nullptr);

			for (auto& itr : vRenderFinishes)
				vkDestroySemaphore(vDevice.GetLogicalDevice(), itr, nullptr);

			for (auto& itr : vInFlightFences)
				vkDestroyFence(vDevice.GetLogicalDevice(), itr, nullptr);
		}
	}
}