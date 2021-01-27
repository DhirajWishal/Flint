// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanDevice.h"
#include "VulkanBackend/VulkanMacros.h"
#include "VulkanBackend/VulkanOneTimeCommandBuffer.h"
#include "VulkanBackend/VulkanUtilities.h"

#include <set>

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{
			/**
			 * Get the max usable sample count of a physical device.
			 *
			 * @param vPhysicalDevice: The physical device.
			 * @return VkSampleCountFlags enum.
			 */
			VkSampleCountFlags GetMaxUsableSamples(VkPhysicalDevice vPhysicalDevice)
			{
				VkPhysicalDeviceProperties physicalDeviceProperties;
				vkGetPhysicalDeviceProperties(vPhysicalDevice, &physicalDeviceProperties);

				VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
				if (counts & VK_SAMPLE_COUNT_64_BIT) return VK_SAMPLE_COUNT_64_BIT;
				if (counts & VK_SAMPLE_COUNT_32_BIT) return VK_SAMPLE_COUNT_32_BIT;
				if (counts & VK_SAMPLE_COUNT_16_BIT) return VK_SAMPLE_COUNT_16_BIT;
				if (counts & VK_SAMPLE_COUNT_8_BIT) return VK_SAMPLE_COUNT_8_BIT;
				if (counts & VK_SAMPLE_COUNT_4_BIT) return VK_SAMPLE_COUNT_4_BIT;
				if (counts & VK_SAMPLE_COUNT_2_BIT) return VK_SAMPLE_COUNT_2_BIT;

				return VK_SAMPLE_COUNT_1_BIT;
			}

			/**
			 * Check if the extensions are supported for the Physical Device.
			 *
			 * @param vPhysicalDevice: The physical device to be checked for.
			 * @param deviceExtensions: The device extensions to be checked with.
			 * @return Boolean value.
			 */
			bool CheckDeviceExtensionSupport(VkPhysicalDevice vPhysicalDevice, std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME })
			{
				UI32 extensionCount = 0;
				vkEnumerateDeviceExtensionProperties(vPhysicalDevice, nullptr, &extensionCount, nullptr);

				std::vector<VkExtensionProperties> availableExtensions(extensionCount);
				vkEnumerateDeviceExtensionProperties(vPhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

				std::set<String> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

				for (const VkExtensionProperties& extension : availableExtensions)
					requiredExtensions.erase(extension.extensionName);

				return requiredExtensions.empty();
			}

			/**
			 * Check if a physical device is suitable to use.
			 *
			 * @param vDevice: The physical device to be checked.
			 * @param vSurface: The surface the device will be using.
			 * @param deviceExtensions: The physical device extensions.
			 * @return Boolean value.
			 */
			bool IsPhysicalDeviceSuitable(VkPhysicalDevice vDevice, VkSurfaceKHR vSurface, const std::vector<const char*>& deviceExtensions)
			{
				VulkanQueue vQueue = {};
				vQueue.Initialize(vDevice, vSurface);

				bool extensionsSupported = _Helpers::CheckDeviceExtensionSupport(vDevice, deviceExtensions);
				bool swapChainAdequate = false;
				if (extensionsSupported)
				{
					SwapChainSupportDetails swapChainSupport = SwapChainSupportDetails::Query(vDevice, vSurface);
					swapChainAdequate = (!swapChainSupport.formats.empty()) && (!swapChainSupport.presentModes.empty());
				}

				VkPhysicalDeviceFeatures supportedFeatures = {};
				vkGetPhysicalDeviceFeatures(vDevice, &supportedFeatures);

				return vQueue.IsComplete()
					&& extensionsSupported
					&& swapChainAdequate
					&& supportedFeatures.samplerAnisotropy;
				return true;
			}
		}

		SwapChainSupportDetails SwapChainSupportDetails::Query(VkPhysicalDevice vPhysicalDevice, VkSurfaceKHR vSurface)
		{
			SwapChainSupportDetails supportDetails = {};
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vPhysicalDevice, vSurface, &supportDetails.capabilities);

			UI32 formatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(vPhysicalDevice, vSurface, &formatCount, nullptr);

			if (formatCount != 0)
			{
				supportDetails.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(vPhysicalDevice, vSurface, &formatCount, supportDetails.formats.data());
			}

			UI32 presentModeCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(vPhysicalDevice, vSurface, &presentModeCount, nullptr);

			if (presentModeCount != 0)
			{
				supportDetails.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(vPhysicalDevice, vSurface, &presentModeCount, supportDetails.presentModes.data());
			}

			return supportDetails;
		}

		void VulkanDevice::Initialize(VulkanDisplay* pDisplay)
		{
			this->pDisplay = pDisplay;
			INSERT_INTO_VECTOR(mDeviceExtensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME);

			CreatePhysicalDevice();
			CreateLogicalDevice();
		}

		void VulkanDevice::Terminate()
		{
			DestroyLogicalDevice();
		}

		UI32 VulkanDevice::FindSupporterBufferCount(UI32 count) const
		{
			if (count == std::numeric_limits<UI32>::max())
				return vSurfaceCapabilities.maxImageCount - 1;
			else if (count == 0)
			{
				UI32 bufferCount = vSurfaceCapabilities.minImageCount + 1;
				if (vSurfaceCapabilities.maxImageCount > 0
					&& bufferCount > vSurfaceCapabilities.maxImageCount)
					bufferCount = vSurfaceCapabilities.maxImageCount;

				return bufferCount;
			}

			return count;
		}

		void VulkanDevice::CreatePhysicalDevice()
		{
			VulkanDisplay& display = *pDisplay;
			VulkanInstance& instance = *display.GetInstance();

			UI32 deviceCount = 0;
			vkEnumeratePhysicalDevices(instance.GetInstance(), &deviceCount, nullptr);

			if (deviceCount == 0)
			{
				FLINT_LOG_ERROR(TEXT("Failed to find GPUs with Vulkan support!"))
					return;
			}

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(instance.GetInstance(), &deviceCount, devices.data());

			// Iterate through all the candidate devices and find the best device.
			for (const VkPhysicalDevice& device : devices)
			{
				if (_Helpers::IsPhysicalDeviceSuitable(device, display.GetSurface(), mDeviceExtensions))
				{
					vkGetPhysicalDeviceProperties(device, &vPhysicalDeviceProperties);

					if (vPhysicalDeviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
						vPhysicalDevice = device;
					else if (vPhysicalDeviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
						vPhysicalDevice = device;
					else
						vPhysicalDevice = device;

					break;
				}

				vPhysicalDevice = VK_NULL_HANDLE;
			}

			//  Check if a physical device was found.
			if (vPhysicalDevice == VK_NULL_HANDLE)
			{
				FLINT_LOG_ERROR(TEXT("A suitable physical device was not found!"))
					return;
			}

#ifdef FLINT_DEBUG
			wprintf(TEXT("\n\t---------- VULKAN PHYSICAL DEVICE INFO ----------\n"));
			wprintf(TEXT("API Version: %I32d\n"), vPhysicalDeviceProperties.apiVersion);
			wprintf(TEXT("Driver Version: %I32d\n"), vPhysicalDeviceProperties.driverVersion);
			wprintf(TEXT("Vendor ID: %I32d\n"), vPhysicalDeviceProperties.vendorID);
			wprintf(TEXT("Device ID: %I32d\n"), vPhysicalDeviceProperties.deviceID);

			switch (vPhysicalDeviceProperties.deviceType)
			{
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_OTHER:
				wprintf(TEXT("Device Type: Other GPU\n"));
				break;
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				wprintf(TEXT("Device Type: Discrete GPU (dedicated)\n"));
				break;
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				wprintf(TEXT("Device Type: Integrated GPU (onboard)\n"));
				break;
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
				wprintf(TEXT("Device Type: Virtual GPU\n"));
				break;
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_CPU:
				wprintf(TEXT("Device Type: CPU\n"));
				break;
			default:
				wprintf(TEXT("Device Type: *UNIDENTIFIED\n"));
				break;
			}

			wprintf(TEXT("Device Name: %hs\n"), vPhysicalDeviceProperties.deviceName);
			wprintf(TEXT("\t-------------------------------------------------\n\n"));

#endif	// FLINT_DEBUG

			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vPhysicalDevice, display.GetSurface(), &vSurfaceCapabilities);
			vSwapChainSupportDetails = SwapChainSupportDetails::Query(vPhysicalDevice, display.GetSurface());

			vSampleCount = _Helpers::GetMaxUsableSamples(vPhysicalDevice);
		}

		void VulkanDevice::CreateLogicalDevice()
		{
			VulkanDisplay& display = *pDisplay;
			VulkanInstance& instance = *display.GetInstance();

			vQueue.Initialize(vPhysicalDevice, display.GetSurface());

			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			std::set<UI32> uniqueQueueFamilies = {
				vQueue.mGraphicsFamily.value(),
				vQueue.mComputeFamily.value(),
				vQueue.mTransferFamily.value()
			};

			float queuePriority = 1.0f;
			for (UI32 queueFamily : uniqueQueueFamilies)
			{
				VkDeviceQueueCreateInfo queueCreateInfo = {};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamily;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;
				queueCreateInfos.push_back(queueCreateInfo);
			}

			// Physical device features.
			VkPhysicalDeviceFeatures deviceFeatures = {};
			deviceFeatures.samplerAnisotropy = VK_TRUE;
			deviceFeatures.sampleRateShading = VK_TRUE; // Enable sample shading feature for the device

			// Device create info.
			VkDeviceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.queueCreateInfoCount = static_cast<UI32>(queueCreateInfos.size());
			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.pEnabledFeatures = &deviceFeatures;
			createInfo.enabledExtensionCount = static_cast<UI32>(mDeviceExtensions.size());
			createInfo.ppEnabledExtensionNames = mDeviceExtensions.data();

			std::vector<const char*>& validationLayers = instance.GetValidationLayers();
			if (instance.IsValidationEnabled())
			{
				createInfo.enabledLayerCount = static_cast<UI32>(validationLayers.size());
				createInfo.ppEnabledLayerNames = validationLayers.data();
			}
			else
				createInfo.enabledLayerCount = 0;

			// Create the logical device.
			FLINT_VK_ASSERT(vkCreateDevice(vPhysicalDevice, &createInfo, nullptr, &vLogicalDevice), "Failed to create logical device!")

				// Load the device table.
				volkLoadDeviceTable(&mTable, vLogicalDevice);

			// Get graphics queue.
			mTable.vkGetDeviceQueue(GetLogicalDevice(), vQueue.mGraphicsFamily.value(), 0, &vQueue.vGraphicsQueue);

			// Get compute queue.
			mTable.vkGetDeviceQueue(GetLogicalDevice(), vQueue.mComputeFamily.value(), 0, &vQueue.vComputeQueue);

			// Get transfer queue.
			mTable.vkGetDeviceQueue(GetLogicalDevice(), vQueue.mTransferFamily.value(), 0, &vQueue.vTransferQueue);
		}

		void VulkanDevice::DestroyLogicalDevice()
		{
			mTable.vkDestroyDevice(vLogicalDevice, nullptr);
		}

		VkResult VulkanDevice::MapMemory(VkDeviceMemory vDeviceMemory, UI64 size, UI64 offset, void** ppData) const
		{
			return mTable.vkMapMemory(GetLogicalDevice(), vDeviceMemory, offset, size, 0, ppData);
		}

		void VulkanDevice::UnmapMemory(VkDeviceMemory vDeviceMemory) const
		{
			mTable.vkUnmapMemory(GetLogicalDevice(), vDeviceMemory);
		}

		void VulkanDevice::FreeMemory(VkDeviceMemory vDeviceMemory) const
		{
			mTable.vkFreeMemory(GetLogicalDevice(), vDeviceMemory, nullptr);
		}

		VkResult VulkanDevice::SubmitQueue(VkQueue vQueue, const std::vector<VkSubmitInfo>& vSubmitInfos, VkFence vFence) const
		{
			return mTable.vkQueueSubmit(vQueue, static_cast<UI32>(vSubmitInfos.size()), vSubmitInfos.data(), vFence);
		}

		VkResult VulkanDevice::QueueWait(VkQueue vQueue) const
		{
			return mTable.vkQueueWaitIdle(vQueue);
		}

		VkResult VulkanDevice::WaitIdle() const
		{
			return mTable.vkDeviceWaitIdle(GetLogicalDevice());
		}

		VkResult VulkanDevice::CreateSwapChain(VkSwapchainCreateInfoKHR* pCreateInfo, VkSwapchainKHR* pSwapChain) const
		{
			return mTable.vkCreateSwapchainKHR(GetLogicalDevice(), pCreateInfo, nullptr, pSwapChain);
		}

		VkResult VulkanDevice::GetSwapChainImages(VkSwapchainKHR vSwapChain, UI32* pSwapChainImageCount, std::vector<VkImage>& vImages) const
		{
			return mTable.vkGetSwapchainImagesKHR(GetLogicalDevice(), vSwapChain, pSwapChainImageCount, vImages.data());
		}

		void VulkanDevice::DestroySwapChain(VkSwapchainKHR vSwapChain) const
		{
			mTable.vkDestroySwapchainKHR(GetLogicalDevice(), vSwapChain, nullptr);
		}

		VkResult VulkanDevice::CreateImage(const VkImageCreateInfo* pCreateInfo, VkImage* pImage) const
		{
			return mTable.vkCreateImage(GetLogicalDevice(), pCreateInfo, nullptr, pImage);
		}

		VkResult VulkanDevice::CreateImageMemory(const std::vector<VkImage>& vImages, VkMemoryPropertyFlags vMemoryflags, VkDeviceMemory* pDeviceMemory) const
		{
			if (!vImages.size())
				return VkResult::VK_ERROR_UNKNOWN;

			VkMemoryRequirements vMR = {};
			mTable.vkGetImageMemoryRequirements(GetLogicalDevice(), vImages[0], &vMR);

			VkPhysicalDeviceMemoryProperties vMP = {};
			vkGetPhysicalDeviceMemoryProperties(GetPhysicalDevice(), &vMP);

			VkMemoryAllocateInfo vAI = {};
			vAI.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			vAI.allocationSize = vMR.size * vImages.size();

			for (UI32 i = 0; i < vMP.memoryTypeCount; i++)
			{
				if ((vMR.memoryTypeBits & (1 << i)) && (vMP.memoryTypes[i].propertyFlags & vMemoryflags) == vMemoryflags)
				{
					vAI.memoryTypeIndex = i;
					break;
				}
			}

			FLINT_VK_ASSERT(mTable.vkAllocateMemory(GetLogicalDevice(), &vAI, nullptr, pDeviceMemory), "Failed to allocate image memory!")

				VkResult result = VkResult::VK_ERROR_UNKNOWN;
			for (UI32 i = 0; i < vImages.size(); i++)
				result = mTable.vkBindImageMemory(GetLogicalDevice(), vImages[i], *pDeviceMemory, vMR.size * i);

			return result;
		}

		void VulkanDevice::DestroyImage(VkImage vImage) const
		{
			mTable.vkDestroyImage(GetLogicalDevice(), vImage, nullptr);
		}

		void VulkanDevice::SetImageLayout(VkCommandBuffer vCommandBuffer, VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount, UI32 currentLayer, UI32 mipLevels) const
		{
			VkImageMemoryBarrier vMB = {};
			vMB.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			vMB.oldLayout = vOldLayout;
			vMB.newLayout = vNewLayout;
			vMB.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			vMB.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			vMB.image = vImage;

			if (vNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			{
				vMB.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

				if (Utilities::HasStencilComponent(vFormat))
					vMB.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
			else
				vMB.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

			vMB.subresourceRange.baseMipLevel = 0;
			vMB.subresourceRange.levelCount = mipLevels;
			vMB.subresourceRange.layerCount = layerCount;
			vMB.subresourceRange.baseArrayLayer = currentLayer;
			vMB.srcAccessMask = 0; // TODO
			vMB.dstAccessMask = 0; // TODO

			VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

			switch (vOldLayout)
			{
			case VK_IMAGE_LAYOUT_UNDEFINED:
				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				vMB.srcAccessMask = 0;
				break;

			case VK_IMAGE_LAYOUT_PREINITIALIZED:
				vMB.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				vMB.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				vMB.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				vMB.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				vMB.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				vMB.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;
			default:
				FLINT_LOG_FATAL(TEXT("Unsupported layout transition!"))
					break;
			}

			switch (vNewLayout)
			{
			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				vMB.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				vMB.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				vMB.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				vMB.dstAccessMask = vMB.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				if (vMB.srcAccessMask == 0)
					vMB.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;

				vMB.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				break;

			case VK_IMAGE_LAYOUT_GENERAL:
				destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
				break;
			default:
				FLINT_LOG_FATAL(TEXT("Unsupported layout transition!"))
					break;
			}

			mTable.vkCmdPipelineBarrier(vCommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &vMB);
		}

		void VulkanDevice::SetImageLayout(VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount, UI32 currentLayer, UI32 mipLevels) const
		{
			VulkanOneTimeCommandBuffer vCommandBuffer(this);
			SetImageLayout(vCommandBuffer, vImage, vOldLayout, vNewLayout, vFormat, layerCount, currentLayer, mipLevels);
		}

		VkResult VulkanDevice::CreateImageView(const VkImageViewCreateInfo* pCreateInfo, VkImageView* pImageView) const
		{
			return mTable.vkCreateImageView(GetLogicalDevice(), pCreateInfo, nullptr, pImageView);
		}

		void VulkanDevice::DestroyImageView(VkImageView vImageView) const
		{
			mTable.vkDestroyImageView(GetLogicalDevice(), vImageView, nullptr);
		}

		VkResult VulkanDevice::CreateCommanPool(VkCommandPoolCreateInfo* pCreateInfo, VkCommandPool* pPool) const
		{
			return mTable.vkCreateCommandPool(GetLogicalDevice(), pCreateInfo, nullptr, pPool);
		}

		VkResult VulkanDevice::AllocateCommandBuffers(VkCommandBufferAllocateInfo* pAllicateInfo, const std::vector<VkCommandBuffer>& commandBuffers) const
		{
			return mTable.vkAllocateCommandBuffers(GetLogicalDevice(), pAllicateInfo, const_cast<VkCommandBuffer*>(commandBuffers.data()));
		}

		VkResult VulkanDevice::BeginCommandBuffer(VkCommandBuffer vCommandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) const
		{
			return mTable.vkBeginCommandBuffer(vCommandBuffer, pBeginInfo);
		}

		VkResult VulkanDevice::EndCommandBuffer(VkCommandBuffer vCommandBuffer) const
		{
			return mTable.vkEndCommandBuffer(vCommandBuffer);
		}

		VkResult VulkanDevice::ResetCommandBuffer(VkCommandBuffer vCommandBuffer, VkCommandBufferResetFlags vResetFlags) const
		{
			return mTable.vkResetCommandBuffer(vCommandBuffer, vResetFlags);
		}

		void VulkanDevice::FreeComandBuffers(VkCommandPool vCommandPool, const std::vector<VkCommandBuffer>& vCommandBuffers) const
		{
			mTable.vkFreeCommandBuffers(GetLogicalDevice(), vCommandPool, static_cast<UI32>(vCommandBuffers.size()), vCommandBuffers.data());
		}

		void VulkanDevice::DestroyCommandPool(VkCommandPool vCommandPool) const
		{
			mTable.vkDestroyCommandPool(GetLogicalDevice(), vCommandPool, nullptr);
		}

		VkResult VulkanDevice::CreateRenderPass(const VkRenderPassCreateInfo* pCreateInfo, VkRenderPass* pRenderPass) const
		{
			return mTable.vkCreateRenderPass(GetLogicalDevice(), pCreateInfo, nullptr, pRenderPass);
		}

		void VulkanDevice::DestroyRenderPass(VkRenderPass vRenderPass) const
		{
			mTable.vkDestroyRenderPass(GetLogicalDevice(), vRenderPass, nullptr);
		}

		VkResult VulkanDevice::CreateFrameBuffer(const VkFramebufferCreateInfo* pCreateInfo, VkFramebuffer* pFrameBuffer) const
		{
			return mTable.vkCreateFramebuffer(GetLogicalDevice(), pCreateInfo, nullptr, pFrameBuffer);
		}

		void VulkanDevice::DestroyFrameBuffers(const std::vector<VkFramebuffer>& vFrameBuffers) const
		{
			for (auto itr = vFrameBuffers.begin(); itr != vFrameBuffers.end(); itr++)
				mTable.vkDestroyFramebuffer(GetLogicalDevice(), *itr, nullptr);
		}

		VkResult VulkanDevice::CreateBuffer(const VkBufferCreateInfo* pCreateInfo, VkBuffer* pBuffer) const
		{
			return mTable.vkCreateBuffer(GetLogicalDevice(), pCreateInfo, nullptr, pBuffer);
		}

		VkResult VulkanDevice::CreateBufferMemory(const std::vector<VkBuffer>& vBuffers, VkMemoryPropertyFlags vMemoryflags, VkDeviceMemory* pDeviceMemory) const
		{
			if (!vBuffers.size())
				return VkResult::VK_ERROR_UNKNOWN;

			VkMemoryRequirements vMR = {};
			mTable.vkGetBufferMemoryRequirements(GetLogicalDevice(), vBuffers[0], &vMR);

			VkPhysicalDeviceMemoryProperties vMP = {};
			vkGetPhysicalDeviceMemoryProperties(GetPhysicalDevice(), &vMP);

			VkMemoryAllocateInfo vAI = {};
			vAI.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			vAI.allocationSize = vMR.size * vBuffers.size();

			for (UI32 i = 0; i < vMP.memoryTypeCount; i++)
			{
				if ((vMR.memoryTypeBits & (1 << i)) && (vMP.memoryTypes[i].propertyFlags & vMemoryflags) == vMemoryflags)
				{
					vAI.memoryTypeIndex = i;
					break;
				}
			}

			FLINT_VK_ASSERT(mTable.vkAllocateMemory(GetLogicalDevice(), &vAI, nullptr, pDeviceMemory), "Failed to allocate image memory!")

				VkResult result = VkResult::VK_ERROR_UNKNOWN;
			for (UI32 i = 0; i < vBuffers.size(); i++)
				result = mTable.vkBindBufferMemory(GetLogicalDevice(), vBuffers[i], *pDeviceMemory, vMR.size * i);

			return result;
		}

		void VulkanDevice::DestroyBuffer(VkBuffer vBuffer) const
		{
			mTable.vkDestroyBuffer(GetLogicalDevice(), vBuffer, nullptr);
		}

		Interface::DeviceHandle CreateDevice(const Interface::DisplayHandle& displayHandle)
		{
			VulkanDevice* pDevice = new VulkanDevice();
			pDevice->Initialize(static_cast<VulkanDisplay*>(HandleToPointer(displayHandle)));

			return PointerToHandle<Interface::DeviceHandle>(pDevice);
		}

		void DestroyDevice(const Interface::DeviceHandle& handle)
		{
			VulkanDevice* pDevice = static_cast<VulkanDevice*>(HandleToPointer(handle));
			pDevice->Terminate();

			delete pDevice;
		}
	}
}