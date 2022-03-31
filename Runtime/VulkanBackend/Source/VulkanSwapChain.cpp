// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanSwapChain.hpp"
#include "VulkanBackend/VulkanUtilities.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace Helpers
		{
			VkPresentModeKHR GetPresentMode(SwapChainPresentMode mode)
			{
				switch (mode)
				{
				case Flint::SwapChainPresentMode::Immediate:
					return VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR;

				case Flint::SwapChainPresentMode::MailBox:
					return VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR;

				case Flint::SwapChainPresentMode::FIFO:
					return VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;

				case Flint::SwapChainPresentMode::RelaxedFIFO:
					return VkPresentModeKHR::VK_PRESENT_MODE_FIFO_RELAXED_KHR;

				case Flint::SwapChainPresentMode::SharedDemandRefresh:
					return VkPresentModeKHR::VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR;

				case Flint::SwapChainPresentMode::SharedContinuousRefresh:
					return VkPresentModeKHR::VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR;
				}

				throw backend_error("Invalid swap chain present mode!");
			}
		}

		VulkanSwapChain::VulkanSwapChain(VulkanDevice* m_pDevice, VulkanDisplay* pDisplay, uint32_t imageCount, const SwapChainPresentMode presentMode)
			: SwapChain(m_pDevice, pDisplay, imageCount, presentMode)
		{
			OPTICK_EVENT();

			CreateSwapChain();
			CreateSyncObjects();

			m_vPresentInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			m_vPresentInfo.pNext = VK_NULL_HANDLE;
			m_vPresentInfo.pResults = VK_NULL_HANDLE;
			m_vPresentInfo.swapchainCount = 1;
			m_vPresentInfo.pSwapchains = &m_vSwapChain;
			m_vPresentInfo.waitSemaphoreCount = 1;
			m_vPresentInfo.pWaitSemaphores = &m_vCurrentRenderFinishedSemaphore;
			m_vPresentInfo.pImageIndices = &m_ImageIndex;
		}

		void VulkanSwapChain::Recreate()
		{
			OPTICK_EVENT();

			m_Extent = pDisplay->GetExtent();

			if (pDisplay->IsDisplayResized())
				pDisplay->StaticCast<VulkanDisplay>().ToggleResize();

			CreateSwapChain();

			m_vPresentInfo.pSwapchains = &m_vSwapChain;
			bShouldRecreate = false;
			m_ImageIndex = 0;
		}

		NextImageInfo VulkanSwapChain::AcquireNextImage(const uint32_t frameIndex)
		{
			OPTICK_EVENT();

			NextImageInfo imageInfo = {};

			m_vCurrentInFlightSemaphore = m_vInFlightSemaphores[frameIndex];
			VkResult result = m_pDevice->StaticCast<VulkanDevice>().GetDeviceTable().vkAcquireNextImageKHR(m_pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), m_vSwapChain, std::numeric_limits<uint64_t>::max(), m_vCurrentInFlightSemaphore, VK_NULL_HANDLE, &m_ImageIndex);
			if (result == VkResult::VK_ERROR_OUT_OF_DATE_KHR || result == VkResult::VK_SUBOPTIMAL_KHR)
				imageInfo.bShouldRecreate = true;

			else FLINT_VK_ASSERT(result);

			m_vCurrentRenderFinishedSemaphore = m_vRenderFinishedSemaphores[m_ImageIndex];
			imageInfo.m_Index = m_ImageIndex;
			return imageInfo;
		}

		void VulkanSwapChain::Terminate()
		{
			DestroySwapChain();
			DestroySyncObjects();

			bIsTerminated = true;
		}

		VkPresentInfoKHR* VulkanSwapChain::PrepareToPresent()
		{
			return &m_vPresentInfo;
		}

		VkFormat VulkanSwapChain::GetImageFormat() const
		{
			return Utilities::GetVulkanFormat(m_PixelForamt);
		}

		VkAttachmentDescription VulkanSwapChain::GetAttachmentDescription() const
		{
			VkAttachmentDescription m_vDesc = {};
			m_vDesc.flags = 0;
			m_vDesc.format = GetImageFormat();
			m_vDesc.samples = VK_SAMPLE_COUNT_1_BIT;
			m_vDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			m_vDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			m_vDesc.loadOp = bShouldClear ? VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR : VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			m_vDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			m_vDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			m_vDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			return m_vDesc;
		}

		VkImageLayout VulkanSwapChain::GetAttachmentLayout() const
		{
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		void VulkanSwapChain::CopyFromImage(VkCommandBuffer m_vCommandBuffer, const VkImage m_vSrcImage, const VkImageLayout m_vSrcLayout, VkOffset3D srcOffset, VkOffset3D dstOffset, const uint32_t index, const VkImageSubresourceLayers m_vSrcSubresourceLayer)
		{
			VkImageCopy m_vImageCopy = {};
			m_vImageCopy.extent.width = m_Extent.m_Width;
			m_vImageCopy.extent.height = m_Extent.m_Height;
			m_vImageCopy.extent.depth = 1;
			m_vImageCopy.srcOffset = srcOffset;
			m_vImageCopy.dstOffset = dstOffset;
			m_vImageCopy.srcSubresource = m_vSrcSubresourceLayer;

			m_vImageCopy.dstSubresource.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			m_vImageCopy.dstSubresource.baseArrayLayer = 0;
			m_vImageCopy.dstSubresource.layerCount = 1;
			m_vImageCopy.dstSubresource.mipLevel = 0;

			VulkanDevice& m_vDevice = m_pDevice->StaticCast<VulkanDevice>();
			const auto m_vImage = m_vImages[index];
			const auto m_vFormat = Utilities::GetVulkanFormat(m_PixelForamt);

			m_vDevice.SetImageLayout(m_vCommandBuffer, m_vImage, VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_vFormat);
			m_vDevice.GetDeviceTable().vkCmdCopyImage(m_vCommandBuffer, m_vSrcImage, m_vSrcLayout, m_vImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &m_vImageCopy);
			m_vDevice.SetImageLayout(m_vCommandBuffer, m_vImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, m_vFormat);
			//m_vDevice.SetImageLayout(m_vCommandBuffer, m_vImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VkImageLayout::VK_IMAGE_LAYOUT_GENERAL, m_vFormat);
		}

		void VulkanSwapChain::CreateSwapChain()
		{
			OPTICK_EVENT();

			auto& m_vDevice = m_pDevice->StaticCast<VulkanDevice>();
			auto& m_vDisplay = pDisplay->StaticCast<VulkanDisplay>();

			SwapChainSupportDetails m_vSupport = SwapChainSupportDetails::Query(m_vDevice.GetPhysicalDevice(), m_vDisplay.GetSurface());
			VkSurfaceFormatKHR surfaceFormat = m_vDisplay.ChooseSurfaceFormat(m_vSupport.m_Formats);

			bool bPresentModeAvailable = false;
			VkPresentModeKHR presentMode = Helpers::GetPresentMode(m_PresentMode);
			for (const auto availablePresentMode : m_vSupport.m_PresentModes)
			{
				if (availablePresentMode == presentMode)
				{
					bPresentModeAvailable = true;
					break;
				}
			}

			if (!bPresentModeAvailable)
				throw backend_error("Requested swap chain present mode is not supported!");

			auto m_vCapabilities = m_vDisplay.GetSurfaceCapabilities(m_vDevice);

			VkCompositeAlphaFlagBitsKHR surfaceComposite = static_cast<VkCompositeAlphaFlagBitsKHR>(m_vCapabilities.supportedCompositeAlpha);
			surfaceComposite = (surfaceComposite & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
				? VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
				: (surfaceComposite & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
				? VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR
				: (surfaceComposite & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
				? VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
				: VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

			m_PixelForamt = Utilities::GetPixelFormat(surfaceFormat.format);

			VkSwapchainCreateInfoKHR m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.flags = 0;
			m_vCreateInfo.surface = m_vDisplay.GetSurface();
			m_vCreateInfo.minImageCount = m_ImageCount;
			m_vCreateInfo.imageFormat = surfaceFormat.format;
			m_vCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
			m_vCreateInfo.imageExtent = { static_cast<uint32_t>(m_Extent.m_Width), static_cast<uint32_t>(m_Extent.m_Height) };
			m_vCreateInfo.imageArrayLayers = 1;
			m_vCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			m_vCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			m_vCreateInfo.queueFamilyIndexCount = 0;
			m_vCreateInfo.pQueueFamilyIndices = nullptr;

			const uint32_t queueFamilyindices[2] = {
				m_vDevice.GetQueue().m_GraphicsFamily.value(),
				m_vDevice.GetQueue().m_TransferFamily.value()
			};

			if (m_vDevice.GetQueue().m_GraphicsFamily != m_vDevice.GetQueue().m_TransferFamily)
			{
				m_vCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				m_vCreateInfo.queueFamilyIndexCount = 2;
				m_vCreateInfo.pQueueFamilyIndices = queueFamilyindices;
			}

			m_vCreateInfo.preTransform = m_vSupport.m_Capabilities.currentTransform;
			m_vCreateInfo.compositeAlpha = surfaceComposite;
			m_vCreateInfo.presentMode = presentMode;
			m_vCreateInfo.clipped = VK_TRUE;
			m_vCreateInfo.oldSwapchain = m_vSwapChain;

			if (!m_vDevice.IsDisplayCompatible(pDisplay))
				throw std::runtime_error("Submitted device and display are incompatible!");

			VkSwapchainKHR m_vNewSwapChain = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkCreateSwapchainKHR(m_vDevice.GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vNewSwapChain));

			if (m_vSwapChain != VK_NULL_HANDLE) DestroySwapChain();
			m_vSwapChain = m_vNewSwapChain;

			m_vCreateInfo.minImageCount = 0;
			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkGetSwapchainImagesKHR(m_vDevice.GetLogicalDevice(), m_vSwapChain, &m_vCreateInfo.minImageCount, nullptr));
			m_vImages.resize(m_vCreateInfo.minImageCount);
			FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkGetSwapchainImagesKHR(m_vDevice.GetLogicalDevice(), m_vSwapChain, &m_vCreateInfo.minImageCount, m_vImages.data()));

			m_vImageViews = std::move(Utilities::CreateImageViews(m_vImages, m_vCreateInfo.imageFormat, m_vDevice));
		}

		void VulkanSwapChain::DestroySwapChain()
		{
			auto& m_vDevice = m_pDevice->StaticCast<VulkanDevice>();

			// Terminate the image views.
			for (auto itr = m_vImageViews.begin(); itr != m_vImageViews.end(); itr++)
				m_vDevice.GetDeviceTable().vkDestroyImageView(m_vDevice.GetLogicalDevice(), *itr, nullptr);

			// Terminate the Swap Chain.
			m_vDevice.GetDeviceTable().vkDestroySwapchainKHR(m_vDevice.GetLogicalDevice(), m_vSwapChain, nullptr);

			m_vSwapChain = VK_NULL_HANDLE;
			m_vImages.clear();
			m_vImageViews.clear();
		}

		void VulkanSwapChain::CreateSyncObjects()
		{
			OPTICK_EVENT();

			auto& m_vDevice = m_pDevice->StaticCast<VulkanDevice>();

			VkSemaphoreCreateInfo m_vCreateInfo = {};
			m_vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			m_vCreateInfo.pNext = VK_NULL_HANDLE;
			m_vCreateInfo.flags = 0;

			m_vInFlightSemaphores.reserve(m_vImages.size());
			m_vRenderFinishedSemaphores.reserve(m_vImages.size());

			for (uint64_t i = 0; i < m_vImages.size(); i++)
			{
				VkSemaphore m_vSemaphore = VK_NULL_HANDLE;

				FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkCreateSemaphore(m_vDevice.GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vSemaphore));
				m_vInFlightSemaphores.emplace_back(m_vSemaphore);

				FLINT_VK_ASSERT(m_vDevice.GetDeviceTable().vkCreateSemaphore(m_vDevice.GetLogicalDevice(), &m_vCreateInfo, nullptr, &m_vSemaphore));
				m_vRenderFinishedSemaphores.emplace_back(m_vSemaphore);
			}
		}

		void VulkanSwapChain::DestroySyncObjects()
		{
			auto& m_vDevice = m_pDevice->StaticCast<VulkanDevice>();

			for (uint64_t i = 0; i < m_vInFlightSemaphores.size(); i++)
			{
				m_vDevice.GetDeviceTable().vkDestroySemaphore(m_vDevice.GetLogicalDevice(), m_vInFlightSemaphores[i], nullptr);
				m_vDevice.GetDeviceTable().vkDestroySemaphore(m_vDevice.GetLogicalDevice(), m_vRenderFinishedSemaphores[i], nullptr);
			}
		}
	}
}