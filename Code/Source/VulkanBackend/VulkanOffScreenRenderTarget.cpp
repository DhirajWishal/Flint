// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanOffScreenRenderTarget.hpp"
#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{
			PixelFormat GetPixelFormat(VkFormat format)
			{
				switch (format)
				{
				case VK_FORMAT_UNDEFINED:
					return PixelFormat::UNDEFINED;

				case VK_FORMAT_R8_SRGB:
					return PixelFormat::R8_SRGB;

				case VK_FORMAT_R8G8_SRGB:
					return PixelFormat::R8G8_SRGB;

				case VK_FORMAT_R8G8B8_SRGB:
					return PixelFormat::R8G8B8_SRGB;

				case VK_FORMAT_R8G8B8A8_SRGB:
					return PixelFormat::R8G8B8A8_SRGB;

				case VK_FORMAT_B8G8R8_SRGB:
					return PixelFormat::B8G8R8_SRGB;

				case VK_FORMAT_B8G8R8A8_SRGB:
					return PixelFormat::B8G8R8A8_SRGB;

				case VK_FORMAT_D16_UNORM:
					return PixelFormat::D16_SINT;

				case VK_FORMAT_D32_SFLOAT:
					return PixelFormat::D32_SFLOAT;

				default:
					FLINT_THROW_BACKEND_ERROR("Unsupported format!");
				}

				return PixelFormat::UNDEFINED;
			}
		}

		VulkanOffScreenRenderTarget::VulkanOffScreenRenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pCommandBufferList, OffScreenRenderTargetAttachment attachments, UI32 threadCount)
			: OffScreenRenderTarget(pDevice, extent, bufferCount, pDevice->CreatePrimaryCommandBufferList(bufferCount), attachments, threadCount), vRenderTarget(pDevice->StaticCast<VulkanDevice>())
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();

			std::vector<VulkanRenderTargetAttachment*> pAttachments;
			if ((mAttachments & OffScreenRenderTargetAttachment::COLOR_BUFFER) == OffScreenRenderTargetAttachment::COLOR_BUFFER)
			{
				// TODO
				//pColorBuffer = std::make_unique<VulkanColorBuffer>(vDevice, extent, bufferCount, pSwapChain->GetFormat());
				//pResults.push_back(pDevice->CreateImage(ImageType::DIMENSIONS_2, ImageUsage::GRAPHICS, FBox3D(mExtent.mWidth, mExtent.mHeight, 1), _Helpers::GetPixelFormat(pColorBuffer->GetFormat()), 1, 1, nullptr));
			}

			if ((mAttachments & OffScreenRenderTargetAttachment::DEPTH_BUFFER) == OffScreenRenderTargetAttachment::DEPTH_BUFFER)
			{
				pDepthBuffer = std::make_unique<VulkanDepthBuffer>(vDevice, extent, bufferCount);
				pAttachments.push_back(pDepthBuffer.get());

				pResults.push_back(pDevice->CreateImage(ImageType::DIMENSIONS_2, ImageUsage::GRAPHICS, FBox3D(mExtent.mWidth, mExtent.mHeight, 1), _Helpers::GetPixelFormat(pDepthBuffer->GetFormat()), 1, 1, nullptr));
			}

			std::vector<VkSubpassDependency> vDependencies{ 2 };

			vDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			vDependencies[0].dstSubpass = 0;
			vDependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			vDependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			vDependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			vDependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			vDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			vDependencies[1].srcSubpass = 0;
			vDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			vDependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			vDependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			vDependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			vDependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			vDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			vRenderTarget.CreateRenderPass(pAttachments, VK_PIPELINE_BIND_POINT_GRAPHICS, vDependencies);
			vRenderTarget.CreateFrameBuffer(pAttachments, extent, bufferCount);
			vRenderTarget.CreateSyncObjects(bufferCount);
		}

		void VulkanOffScreenRenderTarget::Execute(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget)
		{

		}

		void VulkanOffScreenRenderTarget::Terminate()
		{
			vRenderTarget.Terminate();

			if (pColorBuffer)
				pColorBuffer->Terminate();

			if (pDepthBuffer)
				pDepthBuffer->Terminate();

			pDevice->DestroyCommandBufferList(pCommandBufferList);
		}
	}
}