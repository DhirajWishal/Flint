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

		VulkanSwapChain::VulkanSwapChain(Device* pDevice, Display* pDisplay, uint32_t imageCount, const SwapChainPresentMode presentMode)
			: SwapChain(pDevice, pDisplay, imageCount, presentMode)
		{
			OPTICK_EVENT();

			CreateSwapChain();
			CreateSyncObjects();

			vPresentInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			vPresentInfo.pNext = VK_NULL_HANDLE;
			vPresentInfo.pResults = VK_NULL_HANDLE;
			vPresentInfo.swapchainCount = 1;
			vPresentInfo.pSwapchains = &vSwapChain;
			vPresentInfo.waitSemaphoreCount = 1;
			vPresentInfo.pWaitSemaphores = &vCurrentRenderFinishedSemaphore;
			vPresentInfo.pImageIndices = &mImageIndex;
		}

		void VulkanSwapChain::Recreate()
		{
			OPTICK_EVENT();

			mExtent = pDisplay->GetExtent();

			if (pDisplay->IsDisplayResized())
				pDisplay->StaticCast<VulkanDisplay>().ToggleResize();

			CreateSwapChain();

			vPresentInfo.pSwapchains = &vSwapChain;
			bShouldRecreate = false;
			mImageIndex = 0;
		}

		NextImageInfo VulkanSwapChain::AcquireNextImage(const uint32_t frameIndex)
		{
			OPTICK_EVENT();

			NextImageInfo imageInfo = {};

			vCurrentInFlightSemaphore = vInFlightSemaphores[frameIndex];
			VkResult result = pDevice->StaticCast<VulkanDevice>().GetDeviceTable().vkAcquireNextImageKHR(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), vSwapChain, std::numeric_limits<uint64_t>::max(), vCurrentInFlightSemaphore, VK_NULL_HANDLE, &mImageIndex);
			if (result == VkResult::VK_ERROR_OUT_OF_DATE_KHR || result == VkResult::VK_SUBOPTIMAL_KHR)
				imageInfo.bShouldRecreate = true;

			else FLINT_VK_ASSERT(result);

			vCurrentRenderFinishedSemaphore = vRenderFinishedSemaphores[mImageIndex];
			imageInfo.mIndex = mImageIndex;
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
			return &vPresentInfo;
		}

		VkFormat VulkanSwapChain::GetImageFormat() const
		{
			return Utilities::GetVulkanFormat(mPixelForamt);
		}

		VkAttachmentDescription VulkanSwapChain::GetAttachmentDescription() const
		{
			VkAttachmentDescription vDesc = {};
			vDesc.flags = 0;
			vDesc.format = GetImageFormat();
			vDesc.samples = VK_SAMPLE_COUNT_1_BIT;
			vDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			vDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			vDesc.loadOp = bShouldClear ? VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR : VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			vDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			vDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			vDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			return vDesc;
		}

		VkImageLayout VulkanSwapChain::GetAttachmentLayout() const
		{
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		void VulkanSwapChain::CopyFromImage(VkCommandBuffer vCommandBuffer, const VkImage vSrcImage, const VkImageLayout vSrcLayout, VkOffset3D srcOffset, VkOffset3D dstOffset, const uint32_t index, const VkImageSubresourceLayers vSrcSubresourceLayer)
		{
			VkImageCopy vImageCopy = {};
			vImageCopy.extent.width = mExtent.mWidth;
			vImageCopy.extent.height = mExtent.mHeight;
			vImageCopy.extent.depth = 1;
			vImageCopy.srcOffset = srcOffset;
			vImageCopy.dstOffset = dstOffset;
			vImageCopy.srcSubresource = vSrcSubresourceLayer;

			vImageCopy.dstSubresource.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			vImageCopy.dstSubresource.baseArrayLayer = 0;
			vImageCopy.dstSubresource.layerCount = 1;
			vImageCopy.dstSubresource.mipLevel = 0;

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			const auto vImage = vImages[index];
			const auto vFormat = Utilities::GetVulkanFormat(mPixelForamt);

			vDevice.SetImageLayout(vCommandBuffer, vImage, VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, vFormat);
			vDevice.GetDeviceTable().vkCmdCopyImage(vCommandBuffer, vSrcImage, vSrcLayout, vImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vImageCopy);
			vDevice.SetImageLayout(vCommandBuffer, vImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, vFormat);
			//vDevice.SetImageLayout(vCommandBuffer, vImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VkImageLayout::VK_IMAGE_LAYOUT_GENERAL, vFormat);
		}

		void VulkanSwapChain::CreateSwapChain()
		{
			OPTICK_EVENT();

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			auto& vDisplay = pDisplay->StaticCast<VulkanDisplay>();

			SwapChainSupportDetails vSupport = SwapChainSupportDetails::Query(vDevice.GetPhysicalDevice(), vDisplay.GetSurface());
			VkSurfaceFormatKHR surfaceFormat = vDisplay.ChooseSurfaceFormat(vSupport.mFormats);

			bool bPresentModeAvailable = false;
			VkPresentModeKHR presentMode = Helpers::GetPresentMode(mPresentMode);
			for (const auto availablePresentMode : vSupport.mPresentModes)
			{
				if (availablePresentMode == presentMode)
				{
					bPresentModeAvailable = true;
					break;
				}
			}

			if (!bPresentModeAvailable)
				throw backend_error("Requested swap chain present mode is not supported!");

			auto vCapabilities = vDisplay.GetSurfaceCapabilities(vDevice);

			VkCompositeAlphaFlagBitsKHR surfaceComposite = static_cast<VkCompositeAlphaFlagBitsKHR>(vCapabilities.supportedCompositeAlpha);
			surfaceComposite = (surfaceComposite & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
				? VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
				: (surfaceComposite & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
				? VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR
				: (surfaceComposite & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
				? VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
				: VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

			mPixelForamt = Utilities::GetPixelFormat(surfaceFormat.format);

			VkSwapchainCreateInfoKHR vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.surface = vDisplay.GetSurface();
			vCreateInfo.minImageCount = mImageCount;
			vCreateInfo.imageFormat = surfaceFormat.format;
			vCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
			vCreateInfo.imageExtent = { static_cast<uint32_t>(mExtent.mWidth), static_cast<uint32_t>(mExtent.mHeight) };
			vCreateInfo.imageArrayLayers = 1;
			vCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			vCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			vCreateInfo.queueFamilyIndexCount = 0;
			vCreateInfo.pQueueFamilyIndices = nullptr;

			const uint32_t queueFamilyindices[2] = {
				vDevice.GetQueue().mGraphicsFamily.value(),
				vDevice.GetQueue().mTransferFamily.value()
			};

			if (vDevice.GetQueue().mGraphicsFamily != vDevice.GetQueue().mTransferFamily)
			{
				vCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				vCreateInfo.queueFamilyIndexCount = 2;
				vCreateInfo.pQueueFamilyIndices = queueFamilyindices;
			}

			vCreateInfo.preTransform = vSupport.mCapabilities.currentTransform;
			vCreateInfo.compositeAlpha = surfaceComposite;
			vCreateInfo.presentMode = presentMode;
			vCreateInfo.clipped = VK_TRUE;
			vCreateInfo.oldSwapchain = vSwapChain;

			if (!vDevice.IsDisplayCompatible(pDisplay))
				throw std::runtime_error("Submitted device and display are incompatible!");

			VkSwapchainKHR vNewSwapChain = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateSwapchainKHR(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vNewSwapChain));

			if (vSwapChain != VK_NULL_HANDLE) DestroySwapChain();
			vSwapChain = vNewSwapChain;

			vCreateInfo.minImageCount = 0;
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkGetSwapchainImagesKHR(vDevice.GetLogicalDevice(), vSwapChain, &vCreateInfo.minImageCount, nullptr));
			vImages.resize(vCreateInfo.minImageCount);
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkGetSwapchainImagesKHR(vDevice.GetLogicalDevice(), vSwapChain, &vCreateInfo.minImageCount, vImages.data()));

			vImageViews = std::move(Utilities::CreateImageViews(vImages, vCreateInfo.imageFormat, vDevice));
		}

		void VulkanSwapChain::DestroySwapChain()
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();

			// Terminate the image views.
			for (auto itr = vImageViews.begin(); itr != vImageViews.end(); itr++)
				vDevice.GetDeviceTable().vkDestroyImageView(vDevice.GetLogicalDevice(), *itr, nullptr);

			// Terminate the Swap Chain.
			vDevice.GetDeviceTable().vkDestroySwapchainKHR(vDevice.GetLogicalDevice(), vSwapChain, nullptr);

			vSwapChain = VK_NULL_HANDLE;
			vImages.clear();
			vImageViews.clear();
		}

		void VulkanSwapChain::CreateSyncObjects()
		{
			OPTICK_EVENT();

			auto& vDevice = pDevice->StaticCast<VulkanDevice>();

			VkSemaphoreCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;

			vInFlightSemaphores.reserve(vImages.size());
			vRenderFinishedSemaphores.reserve(vImages.size());

			for (uint64_t i = 0; i < vImages.size(); i++)
			{
				VkSemaphore vSemaphore = VK_NULL_HANDLE;

				FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateSemaphore(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vSemaphore));
				vInFlightSemaphores.emplace_back(vSemaphore);

				FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkCreateSemaphore(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vSemaphore));
				vRenderFinishedSemaphores.emplace_back(vSemaphore);
			}
		}

		void VulkanSwapChain::DestroySyncObjects()
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();

			for (uint64_t i = 0; i < vInFlightSemaphores.size(); i++)
			{
				vDevice.GetDeviceTable().vkDestroySemaphore(vDevice.GetLogicalDevice(), vInFlightSemaphores[i], nullptr);
				vDevice.GetDeviceTable().vkDestroySemaphore(vDevice.GetLogicalDevice(), vRenderFinishedSemaphores[i], nullptr);
			}
		}
	}
}