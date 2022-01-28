// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

export module Flint.VulkanBackend.VulkanRenderTarget;

import Flint.VulkanBackend.VulkanDevice;
import Flint.VulkanBackend.VulkanRenderTargetAttachmentInterface;
#include "GraphicsCore/RenderTarget.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanRenderTarget
		{
		public:
			VulkanRenderTarget(VulkanDevice& device);

			void Terminate();

			void CreateRenderPass(std::vector<VulkanRenderTargetAttachmentInterface*> pAttachments, VkPipelineBindPoint vBindPoint, const std::vector<VkSubpassDependency>& vSubpassDependencies);
			void CreateRenderPassWithMultipleSubpasses(std::vector<std::vector<VulkanRenderTargetAttachmentInterface*>> pSubpasses, VkPipelineBindPoint vBindPoint, const std::vector<VkSubpassDependency>& vSubpassDependencies);
			void DestroyRenderPass();

			void CreateFrameBuffer(std::vector<VulkanRenderTargetAttachmentInterface*> pAttachments, const FBox2D& extent, const uint32 bufferCount);
			void DestroyFrameBuffers();

		public:
			VkFramebuffer CreateVulkanFrameBuffer(const FBox2D& extent, const std::vector<VkImageView>& vImageViews);

		public:
			std::vector<VkFramebuffer> vFrameBuffers = {};

			VkRenderPass vRenderPass = VK_NULL_HANDLE;

		private:
			VulkanDevice& vDevice;
		};
	}
}

module: private;

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
			OPTICK_EVENT();

			std::vector<VkAttachmentDescription> vDescriptions;

			std::vector<VkAttachmentReference> vColorAttachmentRef;
			std::vector<VkAttachmentReference> vTempColorAttachmentRef;
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

			bool bHasColorAttachment = false;
			for (auto itr = pAttachments.begin(); itr != pAttachments.end(); itr++)
			{
				if (!(*itr))
					continue;

				vDescriptions.emplace_back((*itr)->GetAttachmentDescription());
				vAR.layout = (*itr)->GetAttachmentLayout();

				switch ((*itr)->GetAttachmentType())
				{
				case Flint::VulkanBackend::RenderTargetAttachmenType::SwapChain:
					vResolveAttachmentRef.emplace_back(vAR);
					break;

				case Flint::VulkanBackend::RenderTargetAttachmenType::ColorBuffer:
					vColorAttachmentRef.emplace_back(vAR);
					bHasColorAttachment = true;
					break;

				case Flint::VulkanBackend::RenderTargetAttachmenType::DepthBuffer:
					vDepthAttachmentRef.emplace_back(vAR);
					break;

				default:
					throw backend_error("Invalid or Undefined render target attachment type!");
				}

				vAR.attachment++;
			}

			vSD.pDepthStencilAttachment = vDepthAttachmentRef.data();

			if (bHasColorAttachment)
			{
				vSD.colorAttachmentCount = static_cast<uint32>(vColorAttachmentRef.size());
				vSD.pColorAttachments = vColorAttachmentRef.data();
				vSD.pResolveAttachments = vResolveAttachmentRef.data();
			}
			else
			{
				vSD.colorAttachmentCount = static_cast<uint32>(vResolveAttachmentRef.size());
				vSD.pColorAttachments = vResolveAttachmentRef.data();
				vSD.pResolveAttachments = vColorAttachmentRef.data();
			}

			VkRenderPassCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			vCreateInfo.flags = 0;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.attachmentCount = static_cast<uint32>(vDescriptions.size());
			vCreateInfo.pAttachments = vDescriptions.data();
			vCreateInfo.subpassCount = 1;
			vCreateInfo.pSubpasses = &vSD;
			vCreateInfo.dependencyCount = static_cast<uint32>(vSubpassDependencies.size());
			vCreateInfo.pDependencies = vSubpassDependencies.data();

			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateRenderPass(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vRenderPass));
		}

		void VulkanRenderTarget::CreateRenderPassWithMultipleSubpasses(std::vector<std::vector<VulkanRenderTargetAttachmentInterface*>> pSubpasses, VkPipelineBindPoint vBindPoint, const std::vector<VkSubpassDependency>& vSubpassDependencies)
		{
			OPTICK_EVENT();

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

					vDescriptions.emplace_back((*itr)->GetAttachmentDescription());
					vAR.layout = (*itr)->GetAttachmentLayout();

					switch ((*itr)->GetAttachmentType())
					{
					case Flint::VulkanBackend::RenderTargetAttachmenType::SwapChain:
						vResolveAttachmentRef.emplace_back(vAR);
						break;

					case Flint::VulkanBackend::RenderTargetAttachmenType::ColorBuffer:
						vColorAttachmentRef.emplace_back(vAR);
						break;

					case Flint::VulkanBackend::RenderTargetAttachmenType::DepthBuffer:
						vDepthAttachmentRef.emplace_back(vAR);
						break;

					default:
						throw backend_error("Invalid or Undefined render target attachment type!");
					}

					vAR.attachment++;
				}

				vSD.colorAttachmentCount = static_cast<uint32>(vColorAttachmentRef.size());
				vSD.pColorAttachments = vColorAttachmentRef.data();
				vSD.pDepthStencilAttachment = vDepthAttachmentRef.data();
				vSD.pResolveAttachments = vResolveAttachmentRef.data();

				vSubpassDescriptions.emplace_back(vSD);

				vColorAttachmentRefs.emplace_back(std::move(vColorAttachmentRef));
				vDepthAttachmentRefs.emplace_back(std::move(vDepthAttachmentRef));
				vResolveAttachmentRefs.emplace_back(std::move(vResolveAttachmentRef));
			}

			VkRenderPassCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			vCreateInfo.flags = 0;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.attachmentCount = static_cast<uint32>(vDescriptions.size());
			vCreateInfo.pAttachments = vDescriptions.data();
			vCreateInfo.subpassCount = static_cast<uint32>(vSubpassDescriptions.size());
			vCreateInfo.pSubpasses = vSubpassDescriptions.data();
			vCreateInfo.dependencyCount = static_cast<uint32>(vSubpassDependencies.size());
			vCreateInfo.pDependencies = vSubpassDependencies.data();

			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateRenderPass(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vRenderPass));
		}

		void VulkanRenderTarget::DestroyRenderPass()
		{
			vDevice.GetDeviceTable().vkDestroyRenderPass(vDevice.GetLogicalDevice(), vRenderPass, nullptr);
		}

		void VulkanRenderTarget::CreateFrameBuffer(std::vector<VulkanRenderTargetAttachmentInterface*> pAttachments, const FBox2D& extent, const uint32 bufferCount)
		{
			OPTICK_EVENT();

			VkFramebufferCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			vCreateInfo.flags = 0;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.layers = 1;
			vCreateInfo.renderPass = vRenderPass;
			vCreateInfo.width = extent.mWidth;
			vCreateInfo.height = extent.mHeight;
			vCreateInfo.attachmentCount = static_cast<uint32>(pAttachments.size());

			vFrameBuffers.resize(bufferCount);
			for (uint32 i = 0; i < bufferCount; i++)
			{
				std::vector<VkImageView> vAttachments;

				for (auto itr = pAttachments.begin(); itr != pAttachments.end(); itr++)
					vAttachments.emplace_back((*itr)->GetImageView(i));

				vCreateInfo.pAttachments = vAttachments.data();
				FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateFramebuffer(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vFrameBuffers[i]));
			}
		}

		void VulkanRenderTarget::DestroyFrameBuffers()
		{
			for (auto itr : vFrameBuffers)
				vDevice.GetDeviceTable().vkDestroyFramebuffer(vDevice.GetLogicalDevice(), itr, nullptr);

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
			vCreateInfo.width = extent.mWidth;
			vCreateInfo.height = extent.mHeight;
			vCreateInfo.attachmentCount = static_cast<uint32>(vImageViews.size());
			vCreateInfo.pAttachments = vImageViews.data();

			VkFramebuffer vFrameBuffer = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateFramebuffer(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vFrameBuffer));

			return vFrameBuffer;
		}
	}
}