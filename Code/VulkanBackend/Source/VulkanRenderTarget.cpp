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
					break;

				case Flint::VulkanBackend::RenderTargetAttachmenType::COLOR_BUFFER:
					INSERT_INTO_VECTOR(vColorAttachmentRef, vAR);
					break;

				case Flint::VulkanBackend::RenderTargetAttachmenType::DEPTH_BUFFER:
					INSERT_INTO_VECTOR(vDepthAttachmentRef, vAR);
					break;

				default:
					throw backend_error("Invalid or Undefined render target attachment type!");
				}

				vAR.attachment++;
			}

			vSD.colorAttachmentCount = static_cast<UI32>(vColorAttachmentRef.size());
			vSD.pColorAttachments = vColorAttachmentRef.data();
			vSD.pDepthStencilAttachment = vDepthAttachmentRef.data();
			vSD.pResolveAttachments = vResolveAttachmentRef.data();

			VkRenderPassCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			vCreateInfo.flags = 0;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.attachmentCount = static_cast<UI32>(vDescriptions.size());
			vCreateInfo.pAttachments = vDescriptions.data();
			vCreateInfo.subpassCount = 1;
			vCreateInfo.pSubpasses = &vSD;
			vCreateInfo.dependencyCount = static_cast<UI32>(vSubpassDependencies.size());
			vCreateInfo.pDependencies = vSubpassDependencies.data();

			FLINT_VK_ASSERT(vkCreateRenderPass(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vRenderPass));
		}

		void VulkanRenderTarget::CreateRenderPassWithMultipleSubpasses(std::vector<std::vector<VulkanRenderTargetAttachmentInterface*>> pSubpasses, VkPipelineBindPoint vBindPoint, const std::vector<VkSubpassDependency>& vSubpassDependencies)
		{
			FLINT_SETUP_PROFILER();

			std::vector<VkAttachmentDescription> vDescriptions;
			std::vector<VkSubpassDescription> vSubpassDescriptions;

			std::vector<std::vector<VkAttachmentReference>> vColorAttachmentRefs;
			std::vector<std::vector<VkAttachmentReference>> vDepthAttachmentRefs;
			std::vector<std::vector<VkAttachmentReference>> vResolveAttachmentRefs;

			for (auto pAttachments : pSubpasses)
			{
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
						break;

					case Flint::VulkanBackend::RenderTargetAttachmenType::COLOR_BUFFER:
						INSERT_INTO_VECTOR(vColorAttachmentRef, vAR);
						break;

					case Flint::VulkanBackend::RenderTargetAttachmenType::DEPTH_BUFFER:
						INSERT_INTO_VECTOR(vDepthAttachmentRef, vAR);
						break;

					default:
						throw backend_error("Invalid or Undefined render target attachment type!");
					}

					vAR.attachment++;
				}

				vSD.colorAttachmentCount = static_cast<UI32>(vColorAttachmentRef.size());
				vSD.pColorAttachments = vColorAttachmentRef.data();
				vSD.pDepthStencilAttachment = vDepthAttachmentRef.data();
				vSD.pResolveAttachments = vResolveAttachmentRef.data();

				vSubpassDescriptions.push_back(vSD);

				vColorAttachmentRefs.push_back(std::move(vColorAttachmentRef));
				vDepthAttachmentRefs.push_back(std::move(vDepthAttachmentRef));
				vResolveAttachmentRefs.push_back(std::move(vResolveAttachmentRef));
			}

			VkRenderPassCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			vCreateInfo.flags = 0;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.attachmentCount = static_cast<UI32>(vDescriptions.size());
			vCreateInfo.pAttachments = vDescriptions.data();
			vCreateInfo.subpassCount = static_cast<UI32>(vSubpassDescriptions.size());
			vCreateInfo.pSubpasses = vSubpassDescriptions.data();
			vCreateInfo.dependencyCount = static_cast<UI32>(vSubpassDependencies.size());
			vCreateInfo.pDependencies = vSubpassDependencies.data();

			FLINT_VK_ASSERT(vkCreateRenderPass(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vRenderPass));
		}

		void VulkanRenderTarget::DestroyRenderPass()
		{
			vkDestroyRenderPass(vDevice.GetLogicalDevice(), vRenderPass, nullptr);
		}

		void VulkanRenderTarget::CreateFrameBuffer(std::vector<VulkanRenderTargetAttachmentInterface*> pAttachments, const FBox2D& extent, const UI32 bufferCount)
		{
			FLINT_SETUP_PROFILER();

			VkFramebufferCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			vCreateInfo.flags = 0;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.layers = 1;
			vCreateInfo.renderPass = vRenderPass;
			vCreateInfo.width = static_cast<UI32>(extent.mWidth);
			vCreateInfo.height = static_cast<UI32>(extent.mHeight);
			vCreateInfo.attachmentCount = static_cast<UI32>(pAttachments.size());

			vFrameBuffers.resize(bufferCount);
			for (UI32 i = 0; i < bufferCount; i++)
			{
				std::vector<VkImageView> vAttachments;

				for (auto itr = pAttachments.begin(); itr != pAttachments.end(); itr++)
					INSERT_INTO_VECTOR(vAttachments, (*itr)->GetImageView(i));

				vCreateInfo.pAttachments = vAttachments.data();
				FLINT_VK_ASSERT(vkCreateFramebuffer(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vFrameBuffers[i]));
			}
		}

		void VulkanRenderTarget::DestroyFrameBuffers()
		{
			for (auto itr : vFrameBuffers)
				vkDestroyFramebuffer(vDevice.GetLogicalDevice(), itr, nullptr);

			vFrameBuffers.clear();
		}

		VkFramebuffer VulkanRenderTarget::CreateVulkanFrameBuffer(const FBox2D& extent, const std::vector<VkImageView>& vImageViews)
		{
			VkFramebufferCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			vCreateInfo.flags = 0;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.layers = 1;
			vCreateInfo.renderPass = vRenderPass;
			vCreateInfo.width = static_cast<UI32>(extent.mWidth);
			vCreateInfo.height = static_cast<UI32>(extent.mHeight);
			vCreateInfo.attachmentCount = static_cast<UI32>(vImageViews.size());
			vCreateInfo.pAttachments = vImageViews.data();

			VkFramebuffer vFrameBuffer = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(vkCreateFramebuffer(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vFrameBuffer));

			return vFrameBuffer;
		}
	}
}