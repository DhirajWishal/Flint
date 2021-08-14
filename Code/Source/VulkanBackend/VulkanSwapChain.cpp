// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanSwapChain.hpp"
#include "VulkanBackend/VulkanUtilities.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanSwapChain::VulkanSwapChain(VulkanDevice& device, VulkanDisplay& display, const FBox2D& extent, const UI32 bufferCount)
			: VulkanRenderTargetAttachment(RenderTargetAttachmenType::SWAP_CHAIN, device, extent, bufferCount), vDisplay(display)
		{
			FLINT_SETUP_PROFILER();

			Initialize();
		}

		void VulkanSwapChain::Recreate(const FBox2D& extent)
		{
			FLINT_SETUP_PROFILER();

			mExtent = extent;

			Initialize();
		}

		void VulkanSwapChain::Terminate()
		{
			// Terminate the image views.
			for (auto itr = vImageViews.begin(); itr != vImageViews.end(); itr++)
				vkDestroyImageView(vDevice.GetLogicalDevice(), *itr, nullptr);

			// Terminate the Swap Chain.
			vkDestroySwapchainKHR(vDevice.GetLogicalDevice(), vSwapChain, nullptr);

			vSwapChain = VK_NULL_HANDLE;
			vImageViews.clear();
			vImages.clear();
		}

		VkAttachmentDescription VulkanSwapChain::GetAttachmentDescription() const
		{
			VkAttachmentDescription vDesc = {};
			vDesc.flags = 0;
			vDesc.format = vFormat;
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

		void VulkanSwapChain::Initialize()
		{
			FLINT_SETUP_PROFILER();

			SwapChainSupportDetails vSupport = SwapChainSupportDetails::Query(vDevice.GetPhysicalDevice(), vDisplay.GetSurface());
			VkSurfaceFormatKHR surfaceFormat = vDisplay.ChooseSurfaceFormat(vSupport.mFormats);
			VkPresentModeKHR presentMode = vDisplay.ChoosePresentMode(vSupport.mPresentModes);

			auto vCapabilities = vDisplay.GetSurfaceCapabilities(vDevice);

			VkCompositeAlphaFlagBitsKHR surfaceComposite = static_cast<VkCompositeAlphaFlagBitsKHR>(vCapabilities.supportedCompositeAlpha);
			surfaceComposite = (surfaceComposite & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
				? VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
				: (surfaceComposite & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
				? VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR
				: (surfaceComposite & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
				? VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
				: VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

			vFormat = surfaceFormat.format;

			VkSwapchainCreateInfoKHR vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			vCreateInfo.flags = 0;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.surface = vDisplay.GetSurface();
			vCreateInfo.minImageCount = mBufferCount;
			vCreateInfo.imageFormat = vFormat;
			vCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
			vCreateInfo.imageExtent = { static_cast<UI32>(mExtent.mWidth), static_cast<UI32>(mExtent.mHeight) };
			vCreateInfo.imageArrayLayers = 1;
			vCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

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
			else
			{
				vCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				vCreateInfo.queueFamilyIndexCount = 0;
				vCreateInfo.pQueueFamilyIndices = nullptr;
			}

			vCreateInfo.preTransform = vSupport.mCapabilities.currentTransform;
			vCreateInfo.compositeAlpha = surfaceComposite;
			vCreateInfo.presentMode = presentMode;
			vCreateInfo.clipped = VK_TRUE;
			vCreateInfo.oldSwapchain = vSwapChain;

			if (!vDevice.IsDisplayCompatible(vDisplay.shared_from_this()))
				FLINT_THROW_RUNTIME_ERROR("Submitted device and display are incompatible!");

			VkSwapchainKHR vNewSwapChain = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(vkCreateSwapchainKHR(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vNewSwapChain));

			if (vSwapChain != VK_NULL_HANDLE) Terminate();
			vSwapChain = vNewSwapChain;

			vCreateInfo.minImageCount = 0;
			FLINT_VK_ASSERT(vkGetSwapchainImagesKHR(vDevice.GetLogicalDevice(), vSwapChain, &vCreateInfo.minImageCount, nullptr));
			vImages.resize(vCreateInfo.minImageCount);
			FLINT_VK_ASSERT(vkGetSwapchainImagesKHR(vDevice.GetLogicalDevice(), vSwapChain, &vCreateInfo.minImageCount, vImages.data()));

			vImageViews = std::move(Utilities::CreateImageViews(vImages, vCreateInfo.imageFormat, vDevice));
		}
	}
}