// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanRasterizer.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanColorAttachment.hpp"
#include "VulkanBackend/VulkanDepthAttachment.hpp"

#include <array>

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanRasterizer::VulkanRasterizer(Engine& engine, uint32_t width, uint32_t height, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions, Multisample multisample /*= Multisample::One*/)
			: Rasterizer(engine, width, height, frameCount, std::move(attachmentDescriptions), multisample)
		{
			// Create the attachments.
			for (const auto attachment : m_AttachmentDescriptions)
			{
				if (attachment.m_Type == AttachmentType::Color)
					m_pAttachments.emplace_back(std::make_unique<VulkanColorAttachment>(getEngine(), width, height, attachment.m_Format, multisample));
				else
					m_pAttachments.emplace_back(std::make_unique<VulkanDepthAttachment>(getEngine(), width, height, attachment.m_Format, multisample));
			}

			// Create the render pass.
			createRenderPass();

			// Create frame buffers.
			createFramebuffers();
		}

		VulkanRasterizer::~VulkanRasterizer()
		{
			// Create the frame buffers.
			createFramebuffers();

			// Destroy render pass.
			destroyRenderPass();
		}

		void VulkanRasterizer::createRenderPass()
		{
			// Create attachment descriptions and references.
			std::vector<VkAttachmentReference> colorAttachments;
			std::vector<VkAttachmentReference> depthAttachments;
			std::vector<VkAttachmentDescription> attachmentDescriptions;

			VkAttachmentReference attachmentReference = {};
			for (const auto& pAttachment : m_pAttachments)
			{
				// Get the attachment.
				const auto pVulkanAttachment = pAttachment->as<VulkanRenderTargetAttachment>();
				attachmentDescriptions.emplace_back(pVulkanAttachment->getAttachmentDescription());

				// Add it as a color attachment if its a color attachment. If not, as a depth attachment.
				if (pVulkanAttachment->getType() == AttachmentType::Color)
				{
					attachmentReference.layout = pVulkanAttachment->getLayout();
					colorAttachments.emplace_back(attachmentReference);
				}
				else
				{
					attachmentReference.layout = pVulkanAttachment->getLayout();
					depthAttachments.emplace_back(attachmentReference);
				}

				// Increment the attachment count.
				attachmentReference.attachment++;
			}

			// Create the subpass dependencies.
			std::array<VkSubpassDependency, 2> subpassDependencies;
			subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependencies[0].dstSubpass = 0;
			subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			subpassDependencies[1].srcSubpass = 0;
			subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			subpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			// Create the subpass description.
			VkSubpassDescription subpassDescription = {};
			subpassDescription.flags = 0;
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.inputAttachmentCount = 0;
			subpassDescription.pInputAttachments = nullptr;
			subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
			subpassDescription.pColorAttachments = colorAttachments.data();
			subpassDescription.pResolveAttachments = nullptr;
			subpassDescription.pDepthStencilAttachment = depthAttachments.data();
			subpassDescription.preserveAttachmentCount = 0;
			subpassDescription.pPreserveAttachments = nullptr;

			// Create the render target.
			VkRenderPassCreateInfo renderPassCreateInfo = {};
			renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassCreateInfo.pNext = nullptr;
			renderPassCreateInfo.flags = 0;
			renderPassCreateInfo.attachmentCount = attachmentReference.attachment;
			renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
			renderPassCreateInfo.subpassCount = 1;
			renderPassCreateInfo.pSubpasses = &subpassDescription;
			renderPassCreateInfo.dependencyCount = 2;
			renderPassCreateInfo.pDependencies = subpassDependencies.data();

			FLINT_VK_ASSERT(getEngineAs<VulkanEngine>().getDeviceTable().vkCreateRenderPass(getEngineAs<VulkanEngine>().getLogicalDevice(), &renderPassCreateInfo, nullptr, &m_RenderPass), "Failed to create render pass!");
		}

		void VulkanRasterizer::destroyRenderPass()
		{
			getEngineAs<VulkanEngine>().getDeviceTable().vkDestroyRenderPass(getEngineAs<VulkanEngine>().getLogicalDevice(), m_RenderPass, nullptr);
		}

		void VulkanRasterizer::createFramebuffers()
		{
			// Get the image views.
			std::vector<VkImageView> imageViews;
			imageViews.reserve(m_pAttachments.size());
			for (const auto& pAttachment : m_pAttachments)
			{
				const auto pVulkanAttachment = pAttachment->as<VulkanRenderTargetAttachment>();
				imageViews.emplace_back(pVulkanAttachment->getImageView());
			}

			VkFramebufferCreateInfo frameBufferCreateInfo = {};
			frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferCreateInfo.pNext = VK_NULL_HANDLE;
			frameBufferCreateInfo.flags = 0;
			frameBufferCreateInfo.renderPass = m_RenderPass;
			frameBufferCreateInfo.attachmentCount = 1;
			frameBufferCreateInfo.width = getWidth();
			frameBufferCreateInfo.height = getHeight();
			frameBufferCreateInfo.layers = 1;
			frameBufferCreateInfo.pAttachments = imageViews.data();

			// Iterate and create the frame buffers.
			m_Framebuffers.resize(getFrameCount());
			for (uint8_t i = 0; i < getFrameCount(); i++)
				FLINT_VK_ASSERT(getEngineAs<VulkanEngine>().getDeviceTable().vkCreateFramebuffer(getEngineAs<VulkanEngine>().getLogicalDevice(), &frameBufferCreateInfo, nullptr, &m_Framebuffers[i]), "Failed to create the frame buffer!");
		}

		void VulkanRasterizer::destroyFramebuffers()
		{
			for (const auto framebuffer : m_Framebuffers)
				getEngineAs<VulkanEngine>().getDeviceTable().vkDestroyFramebuffer(getEngineAs<VulkanEngine>().getLogicalDevice(), framebuffer, nullptr);
		}
	}
}