// Copyright 2021 Dhiraj Wishal
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
				case Flint::SwapChainPresentMode::IMMEDIATE:
					return VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR;

				case Flint::SwapChainPresentMode::MAILBOX:
					return VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR;

				case Flint::SwapChainPresentMode::FIFO:
					return VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;

				case Flint::SwapChainPresentMode::FIFO_RELAXED:
					return VkPresentModeKHR::VK_PRESENT_MODE_FIFO_RELAXED_KHR;

				case Flint::SwapChainPresentMode::SHARED_DEMAND_REFRESH:
					return VkPresentModeKHR::VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR;

				case Flint::SwapChainPresentMode::SHARED_CONTINUOUS_REFRESH:
					return VkPresentModeKHR::VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR;
				}

				FLINT_THROW_BACKEND_ERROR("Invalid swap chain present mode!");
			}
		}

		VulkanSwapChain::VulkanSwapChain(const boost::shared_ptr<Device>& pDevice, const boost::shared_ptr<Display>& pDisplay, UI32 imageCount, const SwapChainPresentMode presentMode)
			: SwapChain(pDevice, pDisplay, imageCount, presentMode)
		{
			FLINT_SETUP_PROFILER();

			CreateSwapChain();
			CreateSyncObjects();

			vPresentInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			vPresentInfo.pNext = VK_NULL_HANDLE;
			vPresentInfo.pResults = VK_NULL_HANDLE;
			vPresentInfo.swapchainCount = 1;
			vPresentInfo.pSwapchains = &vSwapChain;
			vPresentInfo.waitSemaphoreCount = 1;
			vPresentInfo.pWaitSemaphores = &vRenderFinished;
			vPresentInfo.pImageIndices = &mImageIndex;
		}

		void VulkanSwapChain::Recreate()
		{
			FLINT_SETUP_PROFILER();

			mExtent = pDisplay->GetExtent();

			if (pDisplay->IsDisplayResized())
				pDisplay->StaticCast<VulkanDisplay>().ToggleResize();

			CreateSwapChain();

			vPresentInfo.pSwapchains = &vSwapChain;
			bShouldRecreate = false;
			mImageIndex = 0;
		}

		NextImageInfo VulkanSwapChain::AcquireNextImage()
		{
			NextImageInfo imageInfo = {};

			VkResult result = vkAcquireNextImageKHR(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), vSwapChain, UI64_MAX, vInFlightSemaphore, VK_NULL_HANDLE, &mImageIndex);
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
				imageInfo.bShouldRecreate = true;
			else FLINT_VK_ASSERT(result);

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
			vDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			vDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			vDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			vDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			return vDesc;
		}

		VkImageLayout VulkanSwapChain::GetAttachmentLayout() const
		{
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		void VulkanSwapChain::CreateSwapChain()
		{
			FLINT_SETUP_PROFILER();

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
				FLINT_THROW_BACKEND_ERROR("Requested swap chain present mode is not supported!");

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
			vCreateInfo.imageExtent = { static_cast<UI32>(mExtent.mWidth), static_cast<UI32>(mExtent.mHeight) };
			vCreateInfo.imageArrayLayers = 1;
			vCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			vCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			vCreateInfo.queueFamilyIndexCount = 0;
			vCreateInfo.pQueueFamilyIndices = nullptr;

			UI32 queueFamilyindices[2] = {
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
				FLINT_THROW_RUNTIME_ERROR("Submitted device and display are incompatible!");

			VkSwapchainKHR vNewSwapChain = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(vkCreateSwapchainKHR(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vNewSwapChain));

			if (vSwapChain != VK_NULL_HANDLE) DestroySwapChain();
			vSwapChain = vNewSwapChain;

			vCreateInfo.minImageCount = 0;
			FLINT_VK_ASSERT(vkGetSwapchainImagesKHR(vDevice.GetLogicalDevice(), vSwapChain, &vCreateInfo.minImageCount, nullptr));
			vImages.resize(vCreateInfo.minImageCount);
			FLINT_VK_ASSERT(vkGetSwapchainImagesKHR(vDevice.GetLogicalDevice(), vSwapChain, &vCreateInfo.minImageCount, vImages.data()));

			vImageViews = std::move(Utilities::CreateImageViews(vImages, vCreateInfo.imageFormat, vDevice));
		}

		void VulkanSwapChain::DestroySwapChain()
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();

			// Terminate the image views.
			for (auto itr = vImageViews.begin(); itr != vImageViews.end(); itr++)
				vkDestroyImageView(vDevice.GetLogicalDevice(), *itr, nullptr);

			// Terminate the Swap Chain.
			vkDestroySwapchainKHR(vDevice.GetLogicalDevice(), vSwapChain, nullptr);

			vSwapChain = VK_NULL_HANDLE;
			vImages.clear();
			vImageViews.clear();
		}

		void VulkanSwapChain::CreateSyncObjects()
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();

			VkSemaphoreCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;

			FLINT_VK_ASSERT(vkCreateSemaphore(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vInFlightSemaphore));
			FLINT_VK_ASSERT(vkCreateSemaphore(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vRenderFinished));
		}

		void VulkanSwapChain::DestroySyncObjects()
		{
			auto& vDevice = pDevice->StaticCast<VulkanDevice>();
			vkDestroySemaphore(vDevice.GetLogicalDevice(), vInFlightSemaphore, nullptr);
			vkDestroySemaphore(vDevice.GetLogicalDevice(), vRenderFinished, nullptr);
		}
	}
}