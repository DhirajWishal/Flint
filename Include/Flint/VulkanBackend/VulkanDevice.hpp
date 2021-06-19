// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Device.hpp"
#include "VulkanInstance.hpp"
#include "VulkanQueue.hpp"
#include "VulkanDisplay.hpp"
#include "VulkanOneTimeCommandBuffer.hpp"
#include "VulkanCommandBufferList.hpp"
#include "VulkanScreenBoundRenderTarget.hpp"
#include "VulkanStaggingBuffer.hpp"
#include "VulkanUniformBuffer.hpp"
#include "VulkanStorageBuffer.hpp"
#include "VulkanVertexBuffer.hpp"
#include "VulkanIndexBuffer.hpp"
#include "VulkanVertexShader.hpp"
#include "VulkanFragmentShader.hpp"

#include <set>

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{
			VkSampleCountFlags GetMaxUsableSamples(VkPhysicalDevice vPhysicalDevice)
			{
				VkPhysicalDeviceProperties physicalDeviceProperties;
				vkGetPhysicalDeviceProperties(vPhysicalDevice, &physicalDeviceProperties);

				VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
				if (counts & VK_SAMPLE_COUNT_64_BIT)
					return VK_SAMPLE_COUNT_64_BIT;
				if (counts & VK_SAMPLE_COUNT_32_BIT)
					return VK_SAMPLE_COUNT_32_BIT;
				if (counts & VK_SAMPLE_COUNT_16_BIT)
					return VK_SAMPLE_COUNT_16_BIT;
				if (counts & VK_SAMPLE_COUNT_8_BIT)
					return VK_SAMPLE_COUNT_8_BIT;
				if (counts & VK_SAMPLE_COUNT_4_BIT)
					return VK_SAMPLE_COUNT_4_BIT;
				if (counts & VK_SAMPLE_COUNT_2_BIT)
					return VK_SAMPLE_COUNT_2_BIT;

				return VK_SAMPLE_COUNT_1_BIT;
			}

			bool CheckDeviceExtensionSupport(VkPhysicalDevice vPhysicalDevice, std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME})
			{
				UI32 extensionCount = 0;
				vkEnumerateDeviceExtensionProperties(vPhysicalDevice, nullptr, &extensionCount, nullptr);

				std::vector<VkExtensionProperties> availableExtensions(extensionCount);
				vkEnumerateDeviceExtensionProperties(vPhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

				std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

				for (const VkExtensionProperties &extension : availableExtensions)
					requiredExtensions.erase(extension.extensionName);

				return requiredExtensions.empty();
			}

			bool IsPhysicalDeviceSuitable(VkPhysicalDevice vDevice, const std::vector<const char *> &deviceExtensions, DeviceFlags flags)
			{
				VulkanQueue vQueue = {};
				vQueue.Initialize(vDevice, flags);

				bool extensionsSupported = _Helpers::CheckDeviceExtensionSupport(vDevice, deviceExtensions);
				bool swapChainAdequate = false;

				VkPhysicalDeviceFeatures supportedFeatures = {};
				vkGetPhysicalDeviceFeatures(vDevice, &supportedFeatures);

				return vQueue.IsComplete() && extensionsSupported && supportedFeatures.samplerAnisotropy;
				return true;
			}
		}

		class VulkanDevice final : public Device
		{
		public:
			VulkanDevice(Instance &instance, DeviceFlags flags) : Device(instance, flags)
			{
				if ((flags & DeviceFlags::GRAPHICS_COMPATIBLE) == DeviceFlags::GRAPHICS_COMPATIBLE)
					INSERT_INTO_VECTOR(mDeviceExtensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME);

				InitializePhysicalDevice();
				InitializeLogicalDevice();
			}

			virtual bool IsDisplayCompatible(const Display &display) override final
			{
				const VulkanDisplay &vDisplay = display.StaticCast<VulkanDisplay>();
				VkBool32 isSupported = VK_FALSE;
				FLINT_VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(GetPhysicalDevice(), GetQueue().mGraphicsFamily.value(), vDisplay.GetSurface(), &isSupported));
				return isSupported == VK_TRUE;
			}

			virtual CommandBufferList &CreatePrimaryCommandBufferList(UI32 bufferCount) override final
			{
				return *new VulkanCommandBufferList(*this, bufferCount);
			}

			virtual CommandBufferList &CreateSecondaryCommandBufferList(UI32 bufferCount, CommandBufferList &parent) override final
			{
				return *new VulkanCommandBufferList(*this, bufferCount, parent);
			}

			virtual void DestroyCommandBufferList(CommandBufferList &commandBufferList) override final
			{
				TerminateDeviceBoundObject(commandBufferList);
				delete &commandBufferList;
			}

			virtual ScreenBoundRenderTarget &CreateScreenBoundRenderTarget(Display &display, const FExtent2D &extent, const UI32 bufferCount) override final
			{
				return *new VulkanScreenBoundRenderTarget(*this, display, extent, bufferCount);
			}

			virtual void DestroyRenderTarget(RenderTarget &renderTarget) override final
			{
				TerminateDeviceBoundObject(renderTarget);
				delete &renderTarget;
			}

			virtual void WaitIdle() override final
			{
				FLINT_VK_ASSERT(vkDeviceWaitIdle(GetLogicalDevice()));
			}

			virtual void WaitForQueue() override final
			{
				FLINT_VK_ASSERT(vkQueueWaitIdle(GetQueue().vTransferQueue));
			}

			virtual StaggingBuffer &CreateStaggingBuffer(UI64 size) override final
			{
				return *new VulkanStaggingBuffer(*this, size);
			}

			virtual UniformBuffer &CreateUniformBuffer(UI64 size) override final
			{
				return *new VulkanUniformBuffer(*this, size);
			}

			virtual StorageBuffer &CreateStorageBuffer(UI64 size) override final
			{
				return *new VulkanStorageBuffer(*this, size);
			}

			virtual VertexBuffer &CreateVertexBuffer(UI64 size, const VertexDescriptor &descriptor) override final
			{
				return *new VulkanVertexBuffer(*this, size, descriptor);
			}

			virtual IndexBuffer &CreateIndexBuffer(UI64 size, UI64 stride) override final
			{
				return *new VulkanIndexBuffer(*this, size, stride);
			}

			virtual void DestroyBuffer(Buffer &buffer) override final
			{
				TerminateDeviceBoundObject(buffer);
				delete &buffer;
			}

			virtual VertexShader &CreateVertexShader(const std::filesystem::path &path, ShaderCodeType type) override final
			{
				return *new VulkanVertexShader(*this, path, type);
			}

			virtual VertexShader &CreateVertexShader(const std::vector<UI32> &code, ShaderCodeType type = ShaderCodeType::SPIR_V) override final
			{
				return *new VulkanVertexShader(*this, code, type);
			}

			virtual VertexShader &CreateVertexShader(const std::string &code, ShaderCodeType type = ShaderCodeType::GLSL) override final
			{
				return *new VulkanVertexShader(*this, code, type);
			}

			virtual FragmentShader &CreateFragmentShader(const std::filesystem::path &path, ShaderCodeType type) override final
			{
				return *new VulkanFragmentShader(*this, path, type);
			}

			virtual FragmentShader &CreateFragmentShader(const std::vector<UI32> &code, ShaderCodeType type = ShaderCodeType::SPIR_V) override final
			{
				return *new VulkanFragmentShader(*this, code, type);
			}

			virtual FragmentShader &CreateFragmentShader(const std::string &code, ShaderCodeType type = ShaderCodeType::GLSL) override final
			{
				return *new VulkanFragmentShader(*this, code, type);
			}

			virtual void DestroyShader(Shader &shader) override final
			{
				TerminateDeviceBoundObject(shader);
				delete &shader;
			}

			virtual void Terminate() override final
			{
				TerminateLogicalDevice();
			}

		public:
			VkDevice GetLogicalDevice() const noexcept { return vLogicalDevice; }
			VkPhysicalDevice GetPhysicalDevice() const noexcept { return vPhysicalDevice; }

			VulkanQueue &GetQueue() { return vQueue; }
			const VulkanQueue GetQueue() const { return vQueue; }
			VkSampleCountFlags GetSampleCount() const { return vSampleCount; }

			VkResult CreateImageMemory(const std::vector<VkImage> &vImages, VkMemoryPropertyFlags vMemoryflags, VkDeviceMemory *pDeviceMemory) const
			{
				if (!vImages.size())
					return VkResult::VK_ERROR_UNKNOWN;

				VkMemoryRequirements vMR = {};
				vkGetImageMemoryRequirements(GetLogicalDevice(), vImages[0], &vMR);

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

				FLINT_VK_ASSERT(vkAllocateMemory(GetLogicalDevice(), &vAI, nullptr, pDeviceMemory));

				VkResult result = VkResult::VK_ERROR_UNKNOWN;
				for (UI32 i = 0; i < vImages.size(); i++)
					result = vkBindImageMemory(GetLogicalDevice(), vImages[i], *pDeviceMemory, vMR.size * i);

				return result;
			}

			void SetImageLayout(VkCommandBuffer vCommandBuffer, VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, UI32 mipLevels = 1) const
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
					FLINT_THROW_RUNTIME_ERROR("Unsupported layout transition!");
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
					FLINT_THROW_RUNTIME_ERROR("Unsupported layout transition!");
				}

				vkCmdPipelineBarrier(vCommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &vMB);
			}

			void SetImageLayout(VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, UI32 layerCount = 1, UI32 currentLayer = 0, UI32 mipLevels = 1) const
			{
				VulkanOneTimeCommandBuffer vCommandBuffer(*this);
				SetImageLayout(vCommandBuffer, vImage, vOldLayout, vNewLayout, vFormat, layerCount, currentLayer, mipLevels);
			}

		private:
			void InitializePhysicalDevice()
			{
				VulkanInstance &instance = mInstance.StaticCast<VulkanInstance>();

				UI32 deviceCount = 0;
				vkEnumeratePhysicalDevices(instance.GetInstance(), &deviceCount, nullptr);

				if (deviceCount == 0)
					FLINT_THROW_RUNTIME_ERROR("No available devices found!");

				std::vector<VkPhysicalDevice> devices(deviceCount);
				vkEnumeratePhysicalDevices(instance.GetInstance(), &deviceCount, devices.data());

				VkPhysicalDeviceProperties vPhysicalDeviceProperties = {};
				// Iterate through all the candidate devices and find the best device.
				for (const VkPhysicalDevice &device : devices)
				{
					if (_Helpers::IsPhysicalDeviceSuitable(device, mDeviceExtensions, mFlags))
					{
						vkGetPhysicalDeviceProperties(device, &vPhysicalDeviceProperties);

						if (vPhysicalDeviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && (mFlags & DeviceFlags::EXTERNAL) == DeviceFlags::EXTERNAL)
							vPhysicalDevice = device;
						else if (vPhysicalDeviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU && (mFlags & DeviceFlags::INTEGRATED) == DeviceFlags::INTEGRATED)
							vPhysicalDevice = device;
						else
							vPhysicalDevice = device;

						break;
					}

					vPhysicalDevice = VK_NULL_HANDLE;
				}

				//  Check if a physical device was found.
				if (vPhysicalDevice == VK_NULL_HANDLE)
					FLINT_THROW_RUNTIME_ERROR("Unable to find suitable physical device!");

#ifdef FLINT_DEBUG
				printf("\n\t---------- VULKAN PHYSICAL DEVICE INFO ----------\n");
				printf("API Version: %I32d\n", vPhysicalDeviceProperties.apiVersion);
				printf("Driver Version: %I32d\n", vPhysicalDeviceProperties.driverVersion);
				printf("Vendor ID: %I32d\n", vPhysicalDeviceProperties.vendorID);
				printf("Device ID: %I32d\n", vPhysicalDeviceProperties.deviceID);

				switch (vPhysicalDeviceProperties.deviceType)
				{
				case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_OTHER:
					printf("Device Type: Other GPU\n");
					break;
				case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					printf("Device Type: Discrete GPU (dedicated)\n");
					break;
				case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					printf("Device Type: Integrated GPU (on board)\n");
					break;
				case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
					printf("Device Type: Virtual GPU\n");
					break;
				case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_CPU:
					printf("Device Type: CPU\n");
					break;
				default:
					printf("Device Type: *UNIDENTIFIED\n");
					break;
				}

				printf("Device Name: %hs\n", vPhysicalDeviceProperties.deviceName);
				printf("\t------------------------------------------------ - \n\n");

#endif // FLINT_DEBUG

				vSampleCount = _Helpers::GetMaxUsableSamples(vPhysicalDevice);
			}

			void InitializeLogicalDevice()
			{
				VulkanInstance &instance = mInstance.StaticCast<VulkanInstance>();

				vQueue.Initialize(vPhysicalDevice, mFlags);

				std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
				std::set<UI32> uniqueQueueFamilies = {
					vQueue.mGraphicsFamily.value(),
					vQueue.mComputeFamily.value(),
					vQueue.mTransferFamily.value()};

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

				std::vector<const char *> validationLayers = instance.GetValidationLayers();
				if (instance.IsValidationEnabled())
				{
					createInfo.enabledLayerCount = static_cast<UI32>(validationLayers.size());
					createInfo.ppEnabledLayerNames = validationLayers.data();
				}
				else
					createInfo.enabledLayerCount = 0;

				// Create the logical device.
				FLINT_VK_ASSERT(vkCreateDevice(vPhysicalDevice, &createInfo, nullptr, &vLogicalDevice));

				if ((mFlags & DeviceFlags::GRAPHICS_COMPATIBLE) == DeviceFlags::GRAPHICS_COMPATIBLE)
					vkGetDeviceQueue(GetLogicalDevice(), vQueue.mGraphicsFamily.value(), 0, &vQueue.vGraphicsQueue);

				if ((mFlags & DeviceFlags::COMPUTE_COMPATIBLE) == DeviceFlags::COMPUTE_COMPATIBLE)
					vkGetDeviceQueue(GetLogicalDevice(), vQueue.mComputeFamily.value(), 0, &vQueue.vComputeQueue);

				vkGetDeviceQueue(GetLogicalDevice(), vQueue.mTransferFamily.value(), 0, &vQueue.vTransferQueue);
			}

			void TerminateLogicalDevice()
			{
				vkDestroyDevice(vLogicalDevice, nullptr);
			}

		private:
			VulkanQueue vQueue = {};

			std::vector<const char *> mDeviceExtensions;

			VkDevice vLogicalDevice = VK_NULL_HANDLE;
			VkPhysicalDevice vPhysicalDevice = VK_NULL_HANDLE;

			VkSampleCountFlags vSampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;
		};
	}
}