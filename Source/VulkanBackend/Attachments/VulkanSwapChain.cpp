// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/Attachments/VulkanSwapChain.h"
#include "VulkanBackend/VulkanUtilities.h"
#include "VulkanBackend/VulkanMacros.h"
#include "VulkanSwapChain.h"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanSwapChain::VulkanSwapChain(VulkanDevice* pDevice, VulkanDisplay* pDisplay, const FExtent2D& extent, UI32 bufferCount)
			: VulkanRenderTargetAttachment(pDevice, RenderTargetAttachmentType::SWAP_CHAIN, extent, bufferCount), pDisplay(pDisplay)
		{
			SwapChainSupportDetails& vSupport = pDisplay->GetSwapChainSupportDetails(pDevice);
			VkSurfaceFormatKHR surfaceFormat = Utilities::ChooseSwapSurfaceFormat(vSupport.mFormats);
			VkPresentModeKHR presentMode = Utilities::ChooseSwapPresentMode(vSupport.mPresentModes);

			auto& vCapabilities = pDisplay->GetSurfaceCapabilities(pDevice);

			VkCompositeAlphaFlagBitsKHR surfaceComposite = static_cast<VkCompositeAlphaFlagBitsKHR>(vCapabilities.supportedCompositeAlpha);
			surfaceComposite = (surfaceComposite & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
				? VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
				: (surfaceComposite & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
				? VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR
				: (surfaceComposite & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
				? VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
				: VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

			vFormat = surfaceFormat.format;

			VkSwapchainCreateInfoKHR vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			vCI.flags = VK_NULL_HANDLE;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.surface = pDisplay->GetSurface();
			vCI.minImageCount = bufferCount;
			vCI.imageFormat = vFormat;
			vCI.imageColorSpace = surfaceFormat.colorSpace;
			vCI.imageExtent = { static_cast<UI32>(extent.x), static_cast<UI32>(extent.y) };
			vCI.imageArrayLayers = 1;
			vCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			UI32 queueFamilyindices[2] = {
				pDevice->GetQueue().mGraphicsFamily.value(),
				pDevice->GetQueue().mTransferFamily.value()
			};

			if (pDevice->GetQueue().mGraphicsFamily != pDevice->GetQueue().mTransferFamily)
			{
				vCI.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				vCI.queueFamilyIndexCount = 2;
				vCI.pQueueFamilyIndices = queueFamilyindices;
			}
			else
			{
				vCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				vCI.queueFamilyIndexCount = 0;
				vCI.pQueueFamilyIndices = nullptr;
			}

			vCI.preTransform = vSupport.mCapabilities.currentTransform;
			vCI.compositeAlpha = surfaceComposite;
			vCI.presentMode = presentMode;
			vCI.clipped = VK_TRUE;
			vCI.oldSwapchain = vSwapChain;

			if (!pDevice->CheckDisplayCompatibility(*pDisplay))
			{
				Logger::LogFatal(TEXT("The current display does not support Vulkan Swap chain creation! Handle: #8"), static_cast<void*>(vCI.surface));
				return;
			}

			VkSwapchainKHR vNewSwapChain = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(pDevice->CreateSwapChain(&vCI, &vNewSwapChain), "Failed to create the Vulkan Swap Chain!");

			if (vSwapChain != VK_NULL_HANDLE) Terminate();
			vSwapChain = vNewSwapChain;

			vCI.minImageCount = 0;
			FLINT_VK_ASSERT(pDevice->GetSwapChainImages(vSwapChain, &vCI.minImageCount, {}), "Failed to get the Vulkan Swap Chain Images!");
			vImages.resize(vCI.minImageCount);
			FLINT_VK_ASSERT(pDevice->GetSwapChainImages(vSwapChain, &vCI.minImageCount, vImages), "Failed to get the Vulkan Swap Chain Images!");

			vImageViews = std::move(Utilities::CreateImageViews(vImages, vCI.imageFormat, pDevice));
		}

		void VulkanSwapChain::Recreate(const FExtent2D& extent)
		{
			//Terminate(); TODO

			SwapChainSupportDetails& vSupport = pDisplay->GetSwapChainSupportDetails(pDevice);
			VkSurfaceFormatKHR surfaceFormat = Utilities::ChooseSwapSurfaceFormat(vSupport.mFormats);
			VkPresentModeKHR presentMode = Utilities::ChooseSwapPresentMode(vSupport.mPresentModes);

			auto& vCapabilities = pDisplay->GetSurfaceCapabilities(pDevice);

			VkCompositeAlphaFlagBitsKHR surfaceComposite = static_cast<VkCompositeAlphaFlagBitsKHR>(vCapabilities.supportedCompositeAlpha);
			surfaceComposite = (surfaceComposite & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
				? VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
				: (surfaceComposite & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
				? VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR
				: (surfaceComposite & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
				? VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
				: VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

			vFormat = surfaceFormat.format;

			VkSwapchainCreateInfoKHR vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			vCI.flags = VK_NULL_HANDLE;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.surface = pDisplay->GetSurface();
			vCI.minImageCount = mBufferCount;
			vCI.imageFormat = vFormat;
			vCI.imageColorSpace = surfaceFormat.colorSpace;
			vCI.imageExtent = { static_cast<UI32>(extent.x), static_cast<UI32>(extent.y) };
			vCI.imageArrayLayers = 1;
			vCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			UI32 queueFamilyindices[2] = {
				pDevice->GetQueue().mGraphicsFamily.value(),
				pDevice->GetQueue().mTransferFamily.value()
			};

			if (pDevice->GetQueue().mGraphicsFamily != pDevice->GetQueue().mTransferFamily)
			{
				vCI.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				vCI.queueFamilyIndexCount = 2;
				vCI.pQueueFamilyIndices = queueFamilyindices;
			}
			else
			{
				vCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				vCI.queueFamilyIndexCount = 0;
				vCI.pQueueFamilyIndices = nullptr;
			}

			vCI.preTransform = vSupport.mCapabilities.currentTransform;
			vCI.compositeAlpha = surfaceComposite;
			vCI.presentMode = presentMode;
			vCI.clipped = VK_TRUE;
			vCI.oldSwapchain = vSwapChain;

			if (!pDevice->CheckDisplayCompatibility(*pDisplay))
			{
				Logger::LogFatal(TEXT("The current display does not support Vulkan Swap chain creation! Handle: #8"), static_cast<void*>(vCI.surface));
				return;
			}

			VkSwapchainKHR vNewSwapChain = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(pDevice->CreateSwapChain(&vCI, &vNewSwapChain), "Failed to create the Vulkan Swap Chain!");

			if (vSwapChain != VK_NULL_HANDLE) Terminate();
			vSwapChain = vNewSwapChain;

			vCI.minImageCount = 0;
			FLINT_VK_ASSERT(pDevice->GetSwapChainImages(vSwapChain, &vCI.minImageCount, {}), "Failed to get the Vulkan Swap Chain Images!");
			vImages.resize(vCI.minImageCount);
			FLINT_VK_ASSERT(pDevice->GetSwapChainImages(vSwapChain, &vCI.minImageCount, vImages), "Failed to get the Vulkan Swap Chain Images!");

			vImageViews = std::move(Utilities::CreateImageViews(vImages, vCI.imageFormat, pDevice));
		}

		void VulkanSwapChain::Terminate()
		{
			// Terminate the image views.
			for (auto itr = vImageViews.begin(); itr != vImageViews.end(); itr++)
				pDevice->DestroyImageView(*itr);

			// Terminate the Swap Chain.
			pDevice->DestroySwapChain(vSwapChain);

			vSwapChain = VK_NULL_HANDLE;
			vImageViews.clear();
			vImages.clear();
		}

		VkAttachmentDescription VulkanSwapChain::GetAttachmentDescription() const
		{
			VkAttachmentDescription vDesc = {};
			vDesc.flags = VK_NULL_HANDLE;
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
	}
}