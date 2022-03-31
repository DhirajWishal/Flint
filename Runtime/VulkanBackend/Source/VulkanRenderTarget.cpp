// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanRenderTarget.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanRenderTarget::VulkanRenderTarget(VulkanDevice& device)
			: m_vDevice(device)
		{
		}

		void VulkanRenderTarget::Terminate()
		{
			DestroyRenderPass();
			DestroyFrameBuffers();
		}

		void VulkanRenderTarget::CreateRenderPass(std::vector<VulkanRenderTargetAttachmentInterface*> pAttachments, VkPipelineBindPoint m_vBindPoint, const std::vector<VkSubpassDependency>& m_vSubpassDependencies)
		{
			OPTICK_EVENT();

			std::vector<VkAttachmentDescription> m_vDescriptions;

			std::vector<VkAttachmentReference> m_vColorAttachmentRef;
			std::vector<VkAttachmentReference> m_vTempColorAttachmentRef;
			std::vector<VkAttachmentReference> m_vDepthAttachmentRef;
			std::vector<VkAttachmentReference> m_vResolveAttachmentRef;

			VkAttachmentReference m_vAR = {};
			m_vAR.attachment = 0;

			VkSubpassDescription m_vSD = {};
			m_vSD.flags = 0;
			m_vSD.colorAttachmentCount = 0;
			m_vSD.inputAttachmentCount = 0;
			m_vSD.preserveAttachmentCount = 0;
			m_vSD.pipelineBindPoint = m_vBindPoint;

			bool bHasColorAttachment = false;
			for (auto itr = pAttachments.begin(); itr != pAttachments.end(); itr++)
			{
				if (!(*itr))
					continue;

				m_vDescriptions.emplace_back((*itr)->GetAttachmentDescription());
				m_vAR.layout = (*itr)->GetAttachmentLayout();

				switch ((*itr)->GetAttachmentType())
				{
				case Flint::VulkanBackend::RenderTargetAttachmenType::SwapChain:
					m_vResolveAttachmentRef.emplace_back(m_vAR);
					break;

				case Flint::VulkanBackend::RenderTargetAttachmenType::ColorBuffer:
					m_vColorAttachmentRef.emplace_back(m_vAR);
					bHasColorAttachment = true;
					break;

				case Flint::VulkanBackend::RenderTargetAttachmenType::DepthBuffer:
					m_vDepthAttachmentRef.emplace_back(m_vAR);
					break;

				default:
					throw backend_error("Invalid or Undefined render target attachment type!");
				}

				m_vAR.attachment++;
			}

			m_vSD.pDepthStencilAttachment = m_vDepthAttachmentRef.data();

			if (bHasColorAttachment)
			{
				m_vSD.colorAttachmentCount = static_cast<uint32_t>(m_vColorAttachmentRef.size());
				m_vSD.pColorAttachments = m_vColorAttachmentRef.data();
				m_vSD.pResolveAttachments = m_vResolveAttachmentRef.data();
			}
			else
			{
				m_vSD.colorAttachmentCount = static_cast<uint32_t>(m_vResolveAttachmentRef.size());
				m_vSD.pColorAttachments = m_vResolveAttachmentRef.data();
				m_vSD.pResolveAttachments = m_vColorAttachmentRef.data();
			}

			VkRenderPassCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			m_vCreateInfo.flags = 0;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.attachmentCount = static_cast<uint32_t>(m_vDescriptions.size());
			m_vCreateInfo.pAttachments = m_vDescriptions.data();
			m_vCreateInfo.subpassCount = 1;
			m_vCreateInfo.pSubpasses = &m_vSD;
			m_vCreateInfo.dependencyCount = static_cast<uint32_t>(m_vSubpassDependencies.size());
			m_vCreateInfo.pDependencies = m_vSubpassDependencies.data();

			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkCreateRenderPass(m_vDevice.GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vRenderPass));
		}

		void VulkanRenderTarget::CreateRenderPassWithMultipleSubpasses(std::vector<std::vector<VulkanRenderTargetAttachmentInterface*>> pSubpasses, VkPipelineBindPoint m_vBindPoint, const std::vector<VkSubpassDependency>& m_vSubpassDependencies)
		{
			OPTICK_EVENT();

			std::vector<VkAttachmentDescription> m_vDescriptions;
			std::vector<VkSubpassDescription> m_vSubpassDescriptions;

			std::vector<std::vector<VkAttachmentReference>> m_vColorAttachmentRefs;
			std::vector<std::vector<VkAttachmentReference>> m_vDepthAttachmentRefs;
			std::vector<std::vector<VkAttachmentReference>> m_vResolveAttachmentRefs;

			for (auto pAttachments : pSubpasses)
			{
				std::vector<VkAttachmentReference> m_vColorAttachmentRef;
				std::vector<VkAttachmentReference> m_vDepthAttachmentRef;
				std::vector<VkAttachmentReference> m_vResolveAttachmentRef;

				VkAttachmentReference m_vAR = {};
				m_vAR.attachment = 0;

				VkSubpassDescription m_vSD = {};
				m_vSD.flags = 0;
				m_vSD.colorAttachmentCount = 0;
				m_vSD.inputAttachmentCount = 0;
				m_vSD.preserveAttachmentCount = 0;
				m_vSD.pipelineBindPoint = m_vBindPoint;

				for (auto itr = pAttachments.begin(); itr != pAttachments.end(); itr++)
				{
					if (!(*itr))
						continue;

					m_vDescriptions.emplace_back((*itr)->GetAttachmentDescription());
					m_vAR.layout = (*itr)->GetAttachmentLayout();

					switch ((*itr)->GetAttachmentType())
					{
					case Flint::VulkanBackend::RenderTargetAttachmenType::SwapChain:
						m_vResolveAttachmentRef.emplace_back(m_vAR);
						break;

					case Flint::VulkanBackend::RenderTargetAttachmenType::ColorBuffer:
						m_vColorAttachmentRef.emplace_back(m_vAR);
						break;

					case Flint::VulkanBackend::RenderTargetAttachmenType::DepthBuffer:
						m_vDepthAttachmentRef.emplace_back(m_vAR);
						break;

					default:
						throw backend_error("Invalid or Undefined render target attachment type!");
					}

					m_vAR.attachment++;
				}

				m_vSD.colorAttachmentCount = static_cast<uint32_t>(m_vColorAttachmentRef.size());
				m_vSD.pColorAttachments = m_vColorAttachmentRef.data();
				m_vSD.pDepthStencilAttachment = m_vDepthAttachmentRef.data();
				m_vSD.pResolveAttachments = m_vResolveAttachmentRef.data();

				m_vSubpassDescriptions.emplace_back(m_vSD);

				m_vColorAttachmentRefs.emplace_back(std::move(m_vColorAttachmentRef));
				m_vDepthAttachmentRefs.emplace_back(std::move(m_vDepthAttachmentRef));
				m_vResolveAttachmentRefs.emplace_back(std::move(m_vResolveAttachmentRef));
			}

			VkRenderPassCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			m_vCreateInfo.flags = 0;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.attachmentCount = static_cast<uint32_t>(m_vDescriptions.size());
			m_vCreateInfo.pAttachments = m_vDescriptions.data();
			m_vCreateInfo.subpassCount = static_cast<uint32_t>(m_vSubpassDescriptions.size());
			m_vCreateInfo.pSubpasses = m_vSubpassDescriptions.data();
			m_vCreateInfo.dependencyCount = static_cast<uint32_t>(m_vSubpassDependencies.size());
			m_vCreateInfo.pDependencies = m_vSubpassDependencies.data();

			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkCreateRenderPass(m_vDevice.GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vRenderPass));
		}

		void VulkanRenderTarget::DestroyRenderPass()
		{
			m_vDevice.GetDeviceTable().vkDestroyRenderPass(m_vDevice.GetLogicalDevice(), m_vRenderPass, nullptr);
		}

		void VulkanRenderTarget::CreateFrameBuffer(std::vector<VulkanRenderTargetAttachmentInterface*> pAttachments, const FBox2D& extent, const uint32_t bufferCount)
		{
			OPTICK_EVENT();

			VkFramebufferCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			m_vCreateInfo.flags = 0;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.layers = 1;
			m_vCreateInfo.renderPass = m_vRenderPass;
			m_vCreateInfo.width = extent.m_Width;
			m_vCreateInfo.height = extent.m_Height;
			m_vCreateInfo.attachmentCount = static_cast<uint32_t>(pAttachments.size());

			m_vFrameBuffers.resize(bufferCount);
			for (uint32_t i = 0; i < bufferCount; i++)
			{
				std::vector<VkImageView> m_vAttachments;

				for (auto itr = pAttachments.begin(); itr != pAttachments.end(); itr++)
					m_vAttachments.emplace_back((*itr)->GetImageView(i));

				m_vCreateInfo.pAttachments = m_vAttachments.data();
				FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkCreateFramebuffer(m_vDevice.GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vFrameBuffers[i]));
			}
		}

		void VulkanRenderTarget::DestroyFrameBuffers()
		{
			for (auto itr : m_vFrameBuffers)
				m_vDevice.GetDeviceTable().vkDestroyFramebuffer(m_vDevice.GetLogicalDevice(), itr, nullptr);

			m_vFrameBuffers.clear();
		}

		VkFramebuffer VulkanRenderTarget::CreateVulkanFrameBuffer(const FBox2D& extent, const std::vector<VkImageView>& m_vImageViews)
		{
			VkFramebufferCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			m_vCreateInfo.flags = 0;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.layers = 1;
			m_vCreateInfo.renderPass = m_vRenderPass;
			m_vCreateInfo.width = extent.m_Width;
			m_vCreateInfo.height = extent.m_Height;
			m_vCreateInfo.attachmentCount = static_cast<uint32_t>(m_vImageViews.size());
			m_vCreateInfo.pAttachments = m_vImageViews.data();

			VkFramebuffer m_vFrameBuffer = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkCreateFramebuffer(m_vDevice.GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vFrameBuffer));

			return m_vFrameBuffer;
		}
	}
}