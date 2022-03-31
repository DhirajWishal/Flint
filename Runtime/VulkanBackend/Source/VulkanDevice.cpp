// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanDevice.hpp"
#include "VulkanBackend/VulkanDisplay.hpp"
#include "VulkanBackend/VulkanOneTimeCommandBuffer.hpp"
#include "VulkanBackend/VulkanCommandBuffer.hpp"
#include "VulkanBackend/VulkanCommandBufferAllocator.hpp"
#include "VulkanBackend/VulkanSwapChain.hpp"
#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanOffScreenRenderTarget.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanImage.hpp"
#include "VulkanBackend/VulkanImageSampler.hpp"
#include "VulkanBackend/VulkanShader.hpp"
#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "VulkanBackend/VulkanComputePipeline.hpp"
#include "VulkanBackend/VulkanHostSynchronizationPrimitive.hpp"
#include "VulkanBackend/VulkanDeviceSynchronizationPrimitive.hpp"
#include "VulkanBackend/VulkanQuery.hpp"

#include "GraphicsCore/GeometryStore.hpp"

#include <set>

#ifdef FLINT_DEBUG
#include <iostream>

template<size_t Size>
static std::ostream& operator<<(std::ostream& stream, uint8_t(&arr)[Size])
{
	for (size_t i = 0; i < Size; i++)
		stream << arr[i];

	return stream;
}

template<size_t Size>
static std::ostream& operator<<(std::ostream& stream, uint32_t(&arr)[Size])
{
	for (size_t i = 0; i < Size; i++)
		stream << arr[i] << " ";

	return stream;
}

template<size_t Size>
static std::ostream& operator<<(std::ostream& stream, float(&arr)[Size])
{
	for (size_t i = 0; i < Size; i++)
		stream << arr[i] << " ";

	return stream;
}

#endif // FLINT_DEBUG

namespace Flint
{
	namespace VulkanBackend
	{
		namespace Helpers
		{
			VkSampleCountFlags GetMaxUsableSamples(VkPhysicalDevice m_vPhysicalDevice)
			{
				VkPhysicalDeviceProperties physicalDeviceProperties;
				vkGetPhysicalDeviceProperties(m_vPhysicalDevice, &physicalDeviceProperties);

				VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
				if (counts & VK_SAMPLE_COUNT_64_BIT) return VK_SAMPLE_COUNT_64_BIT;
				if (counts & VK_SAMPLE_COUNT_32_BIT) return VK_SAMPLE_COUNT_32_BIT;
				if (counts & VK_SAMPLE_COUNT_16_BIT) return VK_SAMPLE_COUNT_16_BIT;
				if (counts & VK_SAMPLE_COUNT_8_BIT) return VK_SAMPLE_COUNT_8_BIT;
				if (counts & VK_SAMPLE_COUNT_4_BIT) return VK_SAMPLE_COUNT_4_BIT;
				if (counts & VK_SAMPLE_COUNT_2_BIT) return VK_SAMPLE_COUNT_2_BIT;

				return VK_SAMPLE_COUNT_1_BIT;
			}

			bool CheckDeviceExtensionSupport(VkPhysicalDevice m_vPhysicalDevice, std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME })
			{
				uint32_t extensionCount = 0;
				vkEnumerateDeviceExtensionProperties(m_vPhysicalDevice, nullptr, &extensionCount, nullptr);

				std::vector<VkExtensionProperties> availableExtensions(extensionCount);
				vkEnumerateDeviceExtensionProperties(m_vPhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

				std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

				for (const VkExtensionProperties& extension : availableExtensions)
					requiredExtensions.erase(extension.extensionName);

				return requiredExtensions.empty();
			}

			bool IsPhysicalDeviceSuitable(VkPhysicalDevice m_vDevice, const std::vector<const char*>& deviceExtensions, const DeviceFlags flags)
			{
				VulkanQueue m_vQueue = {};
				m_vQueue.Initialize(m_vDevice, flags);

				const bool extensionsSupported = Helpers::CheckDeviceExtensionSupport(m_vDevice, deviceExtensions);

				VkPhysicalDeviceFeatures supportedFeatures = {};
				vkGetPhysicalDeviceFeatures(m_vDevice, &supportedFeatures);

				return m_vQueue.IsComplete()
					&& extensionsSupported
					&& supportedFeatures.samplerAnisotropy;
			}
		}

		VulkanDevice::VulkanDevice(VulkanInstance* pInstance, const DeviceFlags flags) : Device(pInstance, flags)
		{
			OPTICK_EVENT();

			if ((flags & DeviceFlags::GraphicsCompatible) == DeviceFlags::GraphicsCompatible)
				m_DeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

			InitializePhysicalDevice();
			InitializeLogicalDevice();
		}

		bool VulkanDevice::IsDisplayCompatible(const VulkanDisplay* pDisplay)
		{
			OPTICK_EVENT();

			VkBool32 isSupported = VK_FALSE;
			FLINT_VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(GetPhysicalDevice(), GetQueue().m_GraphicsFamily.value(), pDisplay->GetSurface(), &isSupported));
			return isSupported == VK_TRUE;
		}

		MultiSampleCount VulkanDevice::GetSupportedMultiSampleCount() const
		{
			return MultiSampleCount(m_vSampleCount);
		}

		std::unique_ptr<VulkanCommandBufferAllocator> VulkanDevice::CreateCommandBufferAllocator(const uint32_t bufferCount)
		{
			return std::make_unique<VulkanCommandBufferAllocator>(this, bufferCount);
		}

		std::unique_ptr<VulkanCommandBufferAllocator> VulkanDevice::CreateSecondaryCommandBufferAllocator(const uint32_t bufferCount, VulkanCommandBufferAllocator* pParentAllocator)
		{
			return std::make_unique<VulkanCommandBufferAllocator>(this, pParentAllocator, bufferCount);
		}

		std::unique_ptr<VulkanSwapChain> VulkanDevice::CreateSwapChain(VulkanDisplay* pDisplay, uint32_t imageCount, const SwapChainPresentMode presentMode)
		{
			return std::make_unique<VulkanSwapChain>(this, pDisplay, imageCount, presentMode);
		}

		std::unique_ptr<VulkanScreenBoundRenderTarget> VulkanDevice::CreateScreenBoundRenderTarget(VulkanDisplay* pDisplay, const FBox2D& extent, const uint32_t bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments, const SwapChainPresentMode presentMode, const FColor4D& swapChainClearColor)
		{
			return  std::make_unique<VulkanScreenBoundRenderTarget>(this, pDisplay, extent, bufferCount, imageAttachments, presentMode, swapChainClearColor);
		}

		std::unique_ptr<VulkanOffScreenRenderTarget> VulkanDevice::CreateOffScreenRenderTarget(const FBox2D& extent, const uint32_t bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments)
		{
			return std::make_unique<VulkanOffScreenRenderTarget>(this, extent, bufferCount, imageAttachments);
		}

		std::unique_ptr<VulkanBuffer> VulkanDevice::CreateBuffer(const BufferType type, const uint64_t size, const BufferMemoryProfile profile)
		{
			return std::make_unique<VulkanBuffer>(this, type, size, profile);
		}

		std::unique_ptr<VulkanImage> VulkanDevice::CreateImage(const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const uint8_t layers, const uint32_t mipLevels, const void* pImageData, const MultiSampleCount sampleCount)
		{
			return std::make_unique<VulkanImage>(this, type, usage, extent, format, layers, mipLevels, pImageData, sampleCount);
		}

		std::unique_ptr<VulkanImageSampler> VulkanDevice::CreateImageSampler(const ImageSamplerSpecification& specification)
		{
			return std::make_unique<VulkanImageSampler>(this, specification);
		}

		std::unique_ptr<VulkanShader> VulkanDevice::CreateShader(const ShaderType type, const std::filesystem::path& path)
		{
			return std::make_unique<VulkanShader>(this, type, path);
		}

		std::unique_ptr<VulkanShader> VulkanDevice::CreateShader(const ShaderType type, const std::vector<uint32_t>& code)
		{
			return std::make_unique<VulkanShader>(this, type, code);
		}

		std::unique_ptr<VulkanShader> VulkanDevice::CreateShader(const ShaderType type, const std::string& code)
		{
			return std::make_unique<VulkanShader>(this, type, code);
		}

		std::unique_ptr<VulkanGraphicsPipeline> VulkanDevice::CreateGraphicsPipeline(const std::string& pipelineName, VulkanScreenBoundRenderTarget* pScreenBoundRenderTarget, std::unique_ptr<VulkanShader>&& pVertexShader, std::unique_ptr<VulkanShader>&& pTessellationControlShader, std::unique_ptr<VulkanShader>&& pTessellationEvaluationShader, std::unique_ptr<VulkanShader>&& pGeometryShader, std::unique_ptr<VulkanShader>&& pFragmentShader, const GraphicsPipelineSpecification& specification)
		{
			return std::make_unique<VulkanGraphicsPipeline>(this, pipelineName, pScreenBoundRenderTarget, std::move(pVertexShader), std::move(pTessellationControlShader), std::move(pTessellationEvaluationShader), std::move(pGeometryShader), std::move(pFragmentShader), specification);
		}

		std::unique_ptr<VulkanGraphicsPipeline> VulkanDevice::CreateGraphicsPipeline(const std::string& pipelineName, VulkanOffScreenRenderTarget* pOffScreenRenderTarget, std::unique_ptr<VulkanShader>&& pVertexShader, std::unique_ptr<VulkanShader>&& pTessellationControlShader, std::unique_ptr<VulkanShader>&& pTessellationEvaluationShader, std::unique_ptr<VulkanShader>&& pGeometryShader, std::unique_ptr<VulkanShader>&& pFragmentShader, const GraphicsPipelineSpecification& specification)
		{
			return std::make_unique<VulkanGraphicsPipeline>(this, pipelineName, pOffScreenRenderTarget, std::move(pVertexShader), std::move(pTessellationControlShader), std::move(pTessellationEvaluationShader), std::move(pGeometryShader), std::move(pFragmentShader), specification);
		}

		std::unique_ptr<VulkanComputePipeline> VulkanDevice::CreateComputePipeline(const std::string& pipelineName, std::unique_ptr<VulkanShader>&& pShader)
		{
			return std::make_unique<VulkanComputePipeline>(this, pipelineName, std::move(pShader));
		}

		std::shared_ptr<VulkanGeometryStore> VulkanDevice::CreateGeometryStore(const std::vector<ShaderAttribute>& vertexAttributes, uint64_t indexSize, const BufferMemoryProfile profile)
		{
			return std::make_unique<VulkanGeometryStore>(this, vertexAttributes, indexSize, profile);
		}

		std::unique_ptr<VulkanHostSynchronizationPrimitive> VulkanDevice::CreateHostSynchronizationPrimitive()
		{
			return std::make_unique<VulkanHostSynchronizationPrimitive>(this);
		}

		std::unique_ptr<VulkanDeviceSynchronizationPrimitive> VulkanDevice::CreateDeviceSynchronizationPrimitive()
		{
			return std::make_unique<VulkanDeviceSynchronizationPrimitive>(this);
		}

		std::unique_ptr<VulkanQuery> VulkanDevice::CreateQuery(const QueryUsage usage, const uint32_t queryCount)
		{
			return std::make_unique<VulkanQuery>(this, usage, queryCount);
		}

		void VulkanDevice::SubmitGraphicsCommandBuffer(const VulkanCommandBuffer* pCommandBuffer, VulkanHostSynchronizationPrimitive* pPrimitive)
		{
			OPTICK_EVENT();

			VkFence m_vFence = VK_NULL_HANDLE;
			if (pPrimitive)
			{
				pPrimitive->Reset();
				m_vFence = pPrimitive->GetFence();
			}

			FLINT_VK_ASSERT(GetDeviceTable().vkQueueSubmit(GetQueue().m_vGraphicsQueue, 1, pCommandBuffer->GetSubmitInfoAddress(), m_vFence));
		}

		void VulkanDevice::SubmitGraphicsCommandBuffers(const std::vector<VulkanCommandBuffer*>& pCommandBuffers, VulkanHostSynchronizationPrimitive* pPrimitive)
		{
			OPTICK_EVENT();
			const uint64_t bufferCount = pCommandBuffers.size();

			std::vector<VkSubmitInfo> m_vSubmitInfos(bufferCount);
			for (uint64_t i = 0; i < bufferCount; i++)
				m_vSubmitInfos[i] = pCommandBuffers[i]->GetSubmitInfo();

			VkFence m_vFence = VK_NULL_HANDLE;
			if (pPrimitive)
			{
				pPrimitive->Reset();
				m_vFence = pPrimitive->GetFence();
			}

			FLINT_VK_ASSERT(GetDeviceTable().vkQueueSubmit(GetQueue().m_vGraphicsQueue, static_cast<uint32_t>(m_vSubmitInfos.size()), m_vSubmitInfos.data(), m_vFence));
		}

		void VulkanDevice::SubmitComputeCommandBuffer(const VulkanCommandBuffer* pCommandBuffer, VulkanHostSynchronizationPrimitive* pPrimitive)
		{
			OPTICK_EVENT();

			VkFence m_vFence = VK_NULL_HANDLE;
			if (pPrimitive)
			{
				pPrimitive->Reset();
				m_vFence = pPrimitive->GetFence();
			}

			FLINT_VK_ASSERT(GetDeviceTable().vkQueueSubmit(GetQueue().m_vGraphicsQueue, 1, pCommandBuffer->GetSubmitInfoAddress(), m_vFence));
		}

		void VulkanDevice::SubmitComputeCommandBuffers(const std::vector<VulkanCommandBuffer*>& pCommandBuffers, VulkanHostSynchronizationPrimitive* pPrimitive)
		{
			OPTICK_EVENT();
			const uint64_t bufferCount = pCommandBuffers.size();

			std::vector<VkSubmitInfo> m_vSubmitInfos(bufferCount);
			for (uint64_t i = 0; i < bufferCount; i++)
				m_vSubmitInfos[i] = pCommandBuffers[i]->GetSubmitInfo();

			VkFence m_vFence = VK_NULL_HANDLE;
			if (pPrimitive)
			{
				pPrimitive->Reset();
				m_vFence = pPrimitive->GetFence();
			}

			FLINT_VK_ASSERT(GetDeviceTable().vkQueueSubmit(GetQueue().m_vComputeQueue, static_cast<uint32_t>(m_vSubmitInfos.size()), m_vSubmitInfos.data(), m_vFence));
		}

		void VulkanDevice::WaitIdle()
		{
			OPTICK_EVENT();
			FLINT_VK_ASSERT(GetDeviceTable().vkDeviceWaitIdle(GetLogicalDevice()));
		}

		void VulkanDevice::WaitForQueue()
		{
			OPTICK_EVENT();
			FLINT_VK_ASSERT(GetDeviceTable().vkQueueWaitIdle(GetQueue().m_vTransferQueue));
		}

		void VulkanDevice::Terminate()
		{
			TerminateLogicalDevice();

			// Destroy the VMA allocator.
			DestroyVmaAllocator();

			bIsTerminated = true;
		}

		VkResult VulkanDevice::CreateImageMemory(const std::vector<VkImage>& m_vImages, VkMemoryPropertyFlags m_vMemoryFlags, VkDeviceMemory* pDeviceMemory) const
		{
			OPTICK_EVENT();

			if (m_vImages.empty())
				return VkResult::VK_ERROR_UNKNOWN;

			VkMemoryRequirements m_vMR = {};
			GetDeviceTable().vkGetImageMemoryRequirements(GetLogicalDevice(), m_vImages[0], &m_vMR);

			VkPhysicalDeviceMemoryProperties m_vMP = {};
			vkGetPhysicalDeviceMemoryProperties(GetPhysicalDevice(), &m_vMP);

			VkMemoryAllocateInfo m_vAI = {};
			m_vAI.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			m_vAI.allocationSize = m_vMR.size * m_vImages.size();

			for (uint32_t i = 0; i < m_vMP.memoryTypeCount; i++)
			{
				if ((m_vMR.memoryTypeBits & (1 << i)) && (m_vMP.memoryTypes[i].propertyFlags & m_vMemoryFlags) == m_vMemoryFlags)
				{
					m_vAI.memoryTypeIndex = i;
					break;
				}
			}

			FLINT_VK_ASSERT(GetDeviceTable().vkAllocateMemory(GetLogicalDevice(), &m_vAI, nullptr, pDeviceMemory));

			VkResult result = VkResult::VK_ERROR_UNKNOWN;
			for (uint32_t i = 0; i < m_vImages.size(); i++)
				result = GetDeviceTable().vkBindImageMemory(GetLogicalDevice(), m_vImages[i], *pDeviceMemory, m_vMR.size * i);

			return result;
		}

		void VulkanDevice::SetImageLayout(VkCommandBuffer m_vCommandBuffer, VkImage m_vImage, VkImageLayout m_vOldLayout, VkImageLayout m_vNewLayout, VkFormat m_vFormat, uint32_t layerCount, uint32_t currentLayer, const uint32_t mipLevels, const uint32_t currentLevel, VkImageAspectFlags m_vAspects) const
		{
			OPTICK_EVENT();

			VkImageMemoryBarrier m_vMB = {};
			m_vMB.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			m_vMB.oldLayout = m_vOldLayout;
			m_vMB.newLayout = m_vNewLayout;
			m_vMB.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			m_vMB.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			m_vMB.image = m_vImage;

			m_vMB.subresourceRange.aspectMask = m_vAspects;
			if (m_vAspects == VkImageAspectFlagBits::VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM)
			{
				if (m_vOldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
					m_vOldLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL ||
					m_vOldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL ||
					m_vNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
					m_vNewLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL ||
					m_vNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)
				{
					m_vMB.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

					if (Utilities::HasStencilComponent(m_vFormat))
						m_vMB.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
				}
				else
					m_vMB.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			}
			else
			{
				if (Utilities::HasStencilComponent(m_vFormat))
					m_vMB.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}

			m_vMB.subresourceRange.baseMipLevel = currentLevel;
			m_vMB.subresourceRange.levelCount = mipLevels;
			m_vMB.subresourceRange.layerCount = layerCount;
			m_vMB.subresourceRange.baseArrayLayer = currentLayer;
			m_vMB.srcAccessMask = 0;
			m_vMB.dstAccessMask = 0;

			switch (m_vOldLayout)
			{
			case VK_IMAGE_LAYOUT_GENERAL:
			case VK_IMAGE_LAYOUT_UNDEFINED:
				m_vMB.srcAccessMask = 0;
				break;

			case VK_IMAGE_LAYOUT_PREINITIALIZED:
				m_vMB.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				m_vMB.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				m_vMB.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
				m_vMB.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				m_vMB.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
				//m_vMB.srcAccessMask = VK_ACCESS_;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				m_vMB.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				m_vMB.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;

			default:
				throw std::runtime_error("Unsupported layout transition!");
			}

			switch (m_vNewLayout)
			{
			case VK_IMAGE_LAYOUT_UNDEFINED:
			case VK_IMAGE_LAYOUT_GENERAL:
			case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				m_vMB.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				m_vMB.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				m_vMB.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				m_vMB.dstAccessMask = m_vMB.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				m_vMB.srcAccessMask |= VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
				m_vMB.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;

			default:
				throw std::runtime_error("Unsupported layout transition!");
			}

			const VkPipelineStageFlags sourceStage = Utilities::GetPipelineStageFlags(m_vMB.srcAccessMask);
			const VkPipelineStageFlags destinationStage = Utilities::GetPipelineStageFlags(m_vMB.dstAccessMask);
			GetDeviceTable().vkCmdPipelineBarrier(m_vCommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &m_vMB);
		}

		void VulkanDevice::SetImageLayout(VkImage m_vImage, VkImageLayout m_vOldLayout, VkImageLayout m_vNewLayout, VkFormat m_vFormat, uint32_t layerCount, uint32_t currentLayer, const uint32_t mipLevels, const uint32_t currentLevel, VkImageAspectFlags m_vAspects) const
		{
			OPTICK_EVENT();

			const VulkanOneTimeCommandBuffer m_vCommandBuffer(*this);
			SetImageLayout(m_vCommandBuffer, m_vImage, m_vOldLayout, m_vNewLayout, m_vFormat, layerCount, currentLayer, mipLevels, currentLevel, m_vAspects);
		}

		void VulkanDevice::InitializePhysicalDevice()
		{
			OPTICK_EVENT();

			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(pInstance->GetInstance(), &deviceCount, nullptr);

			if (deviceCount == 0)
				throw std::runtime_error("No available devices found!");

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(pInstance->GetInstance(), &deviceCount, devices.data());

			VkPhysicalDeviceProperties m_vPhysicalDeviceProperties = {};
			// Iterate through all the candidate devices and find the best device.
			for (const VkPhysicalDevice& device : devices)
			{
				if (Helpers::IsPhysicalDeviceSuitable(device, m_DeviceExtensions, m_Flags))
				{
					vkGetPhysicalDeviceProperties(device, &m_vPhysicalDeviceProperties);

					if (m_vPhysicalDeviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && (m_Flags & DeviceFlags::External) == DeviceFlags::External)
						m_vPhysicalDevice = device;
					else if (m_vPhysicalDeviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU && (m_Flags & DeviceFlags::Integrated) == DeviceFlags::Integrated)
						m_vPhysicalDevice = device;
					else
						m_vPhysicalDevice = device;

					break;
				}

				m_vPhysicalDevice = VK_NULL_HANDLE;
			}

			//  Check if a physical device was found.
			if (m_vPhysicalDevice == VK_NULL_HANDLE)
				throw std::runtime_error("Unable to find suitable physical device!");

			m_vSampleCount = Helpers::GetMaxUsableSamples(m_vPhysicalDevice);

#ifdef FLINT_DEBUG
			std::cout << "Vulkan Physical Device Information" << std::endl;
			std::cout << "\tAPI Version: " << m_vPhysicalDeviceProperties.apiVersion << std::endl;
			std::cout << "\tDriver Version: " << m_vPhysicalDeviceProperties.driverVersion << std::endl;
			std::cout << "\tVendor ID: " << m_vPhysicalDeviceProperties.vendorID << std::endl;
			std::cout << "\tDevice ID: " << m_vPhysicalDeviceProperties.deviceID << std::endl;
			std::cout << "\tDevice Name: " << m_vPhysicalDeviceProperties.deviceName << std::endl;

			switch (m_vPhysicalDeviceProperties.deviceType)
			{
			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_OTHER:
				std::cout << "\tDevice Type: Other GPU" << std::endl;
				break;

			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				std::cout << "\tDevice Type: Discrete GPU (dedicated)" << std::endl;
				break;

			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				std::cout << "\tDevice Type: Integrated GPU (on board)" << std::endl;
				break;

			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
				std::cout << "\tDevice Type: Virtual GPU" << std::endl;
				break;

			case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_CPU:
				std::cout << "\tDevice Type: CPU" << std::endl;
				break;

			default:
				std::cout << "\tDevice Type: UNIDENTIFIED" << std::endl;
				break;
			}

			std::cout << "\tPipeline Cache UUID: ";
			for (const auto number : m_vPhysicalDeviceProperties.pipelineCacheUUID)
				std::cout << static_cast<uint32_t>(number);
			std::cout << std::endl;

			std::cout << "\tDevice limits:" << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxImageDimension1D: " << m_vPhysicalDeviceProperties.limits.maxImageDimension1D << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxImageDimension2D: " << m_vPhysicalDeviceProperties.limits.maxImageDimension2D << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxImageDimension3D: " << m_vPhysicalDeviceProperties.limits.maxImageDimension3D << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxImageDimensionCube: " << m_vPhysicalDeviceProperties.limits.maxImageDimensionCube << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxImageArrayLayers: " << m_vPhysicalDeviceProperties.limits.maxImageArrayLayers << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTexelBufferElements: " << m_vPhysicalDeviceProperties.limits.maxTexelBufferElements << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxUniformBufferRange: " << m_vPhysicalDeviceProperties.limits.maxUniformBufferRange << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxStorageBufferRange: " << m_vPhysicalDeviceProperties.limits.maxStorageBufferRange << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxPushConstantsSize: " << m_vPhysicalDeviceProperties.limits.maxPushConstantsSize << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxMemoryAllocationCount: " << m_vPhysicalDeviceProperties.limits.maxMemoryAllocationCount << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxSamplerAllocationCount: " << m_vPhysicalDeviceProperties.limits.maxSamplerAllocationCount << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::bufferImageGranularity: " << m_vPhysicalDeviceProperties.limits.bufferImageGranularity << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::sparseAddressSpaceSize: " << m_vPhysicalDeviceProperties.limits.sparseAddressSpaceSize << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxBoundDescriptorSets: " << m_vPhysicalDeviceProperties.limits.maxBoundDescriptorSets << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxPerStageDescriptorSamplers: " << m_vPhysicalDeviceProperties.limits.maxPerStageDescriptorSamplers << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxPerStageDescriptorUniformBuffers: " << m_vPhysicalDeviceProperties.limits.maxPerStageDescriptorUniformBuffers << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxPerStageDescriptorStorageBuffers: " << m_vPhysicalDeviceProperties.limits.maxPerStageDescriptorStorageBuffers << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxPerStageDescriptorSampledImages: " << m_vPhysicalDeviceProperties.limits.maxPerStageDescriptorSampledImages << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxPerStageDescriptorStorageImages: " << m_vPhysicalDeviceProperties.limits.maxPerStageDescriptorStorageImages << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxPerStageDescriptorInputAttachments: " << m_vPhysicalDeviceProperties.limits.maxPerStageDescriptorInputAttachments << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxPerStageResources: " << m_vPhysicalDeviceProperties.limits.maxPerStageResources << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDescriptorSetSamplers: " << m_vPhysicalDeviceProperties.limits.maxDescriptorSetSamplers << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDescriptorSetUniformBuffers: " << m_vPhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffers << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDescriptorSetUniformBuffersDynamic: " << m_vPhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffersDynamic << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDescriptorSetStorageBuffers: " << m_vPhysicalDeviceProperties.limits.maxDescriptorSetStorageBuffers << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDescriptorSetStorageBuffersDynamic: " << m_vPhysicalDeviceProperties.limits.maxDescriptorSetStorageBuffersDynamic << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDescriptorSetSampledImages: " << m_vPhysicalDeviceProperties.limits.maxDescriptorSetSampledImages << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDescriptorSetStorageImages: " << m_vPhysicalDeviceProperties.limits.maxDescriptorSetStorageImages << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDescriptorSetInputAttachments: " << m_vPhysicalDeviceProperties.limits.maxDescriptorSetInputAttachments << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxVertexInputAttributes: " << m_vPhysicalDeviceProperties.limits.maxVertexInputAttributes << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxVertexInputBindings: " << m_vPhysicalDeviceProperties.limits.maxVertexInputBindings << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxVertexInputAttributeOffset: " << m_vPhysicalDeviceProperties.limits.maxVertexInputAttributeOffset << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxVertexInputBindingStride: " << m_vPhysicalDeviceProperties.limits.maxVertexInputBindingStride << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxVertexOutputComponents: " << m_vPhysicalDeviceProperties.limits.maxVertexOutputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTessellationGenerationLevel: " << m_vPhysicalDeviceProperties.limits.maxTessellationGenerationLevel << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTessellationPatchSize: " << m_vPhysicalDeviceProperties.limits.maxTessellationPatchSize << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTessellationControlPerVertexInputComponents: " << m_vPhysicalDeviceProperties.limits.maxTessellationControlPerVertexInputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTessellationControlPerVertexOutputComponents: " << m_vPhysicalDeviceProperties.limits.maxTessellationControlPerVertexOutputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTessellationControlPerPatchOutputComponents: " << m_vPhysicalDeviceProperties.limits.maxTessellationControlPerPatchOutputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTessellationControlTotalOutputComponents: " << m_vPhysicalDeviceProperties.limits.maxTessellationControlTotalOutputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTessellationEvaluationInputComponents: " << m_vPhysicalDeviceProperties.limits.maxTessellationEvaluationInputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTessellationEvaluationOutputComponents: " << m_vPhysicalDeviceProperties.limits.maxTessellationEvaluationOutputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxGeometryShaderInvocations: " << m_vPhysicalDeviceProperties.limits.maxGeometryShaderInvocations << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxGeometryInputComponents: " << m_vPhysicalDeviceProperties.limits.maxGeometryInputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxGeometryOutputComponents: " << m_vPhysicalDeviceProperties.limits.maxGeometryOutputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxGeometryOutputVertices: " << m_vPhysicalDeviceProperties.limits.maxGeometryOutputVertices << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxGeometryTotalOutputComponents: " << m_vPhysicalDeviceProperties.limits.maxGeometryTotalOutputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxFragmentInputComponents: " << m_vPhysicalDeviceProperties.limits.maxFragmentInputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxFragmentOutputAttachments: " << m_vPhysicalDeviceProperties.limits.maxFragmentOutputAttachments << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxFragmentDualSrcAttachments: " << m_vPhysicalDeviceProperties.limits.maxFragmentDualSrcAttachments << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxFragmentCombinedOutputResources: " << m_vPhysicalDeviceProperties.limits.maxFragmentCombinedOutputResources << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxComputeSharedMemorySize: " << m_vPhysicalDeviceProperties.limits.maxComputeSharedMemorySize << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxComputeWorkGroupCount: " << m_vPhysicalDeviceProperties.limits.maxComputeWorkGroupCount << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxComputeWorkGroupInvocations: " << m_vPhysicalDeviceProperties.limits.maxComputeWorkGroupInvocations << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxComputeWorkGroupSize: " << m_vPhysicalDeviceProperties.limits.maxComputeWorkGroupSize << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::subPixelPrecisionBits: " << m_vPhysicalDeviceProperties.limits.subPixelPrecisionBits << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::subTexelPrecisionBits: " << m_vPhysicalDeviceProperties.limits.subTexelPrecisionBits << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::mipmapPrecisionBits: " << m_vPhysicalDeviceProperties.limits.mipmapPrecisionBits << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDrawIndexedIndexValue: " << m_vPhysicalDeviceProperties.limits.maxDrawIndexedIndexValue << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDrawIndirectCount: " << m_vPhysicalDeviceProperties.limits.maxDrawIndirectCount << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxSamplerLodBias: " << m_vPhysicalDeviceProperties.limits.maxSamplerLodBias << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxSamplerAnisotropy: " << m_vPhysicalDeviceProperties.limits.maxSamplerAnisotropy << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxViewports: " << m_vPhysicalDeviceProperties.limits.maxViewports << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxViewportDimensions: " << m_vPhysicalDeviceProperties.limits.maxViewportDimensions << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::viewportBoundsRange: " << m_vPhysicalDeviceProperties.limits.viewportBoundsRange << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::viewportSubPixelBits: " << m_vPhysicalDeviceProperties.limits.viewportSubPixelBits << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::minMemoryMapAlignment: " << m_vPhysicalDeviceProperties.limits.minMemoryMapAlignment << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::minTexelBufferOffsetAlignment: " << m_vPhysicalDeviceProperties.limits.minTexelBufferOffsetAlignment << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::minUniformBufferOffsetAlignment: " << m_vPhysicalDeviceProperties.limits.minUniformBufferOffsetAlignment << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::minStorageBufferOffsetAlignment: " << m_vPhysicalDeviceProperties.limits.minStorageBufferOffsetAlignment << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::minTexelOffset: " << m_vPhysicalDeviceProperties.limits.minTexelOffset << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTexelOffset: " << m_vPhysicalDeviceProperties.limits.maxTexelOffset << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::minTexelGatherOffset: " << m_vPhysicalDeviceProperties.limits.minTexelGatherOffset << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTexelGatherOffset: " << m_vPhysicalDeviceProperties.limits.maxTexelGatherOffset << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::minInterpolationOffset: " << m_vPhysicalDeviceProperties.limits.minInterpolationOffset << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxInterpolationOffset: " << m_vPhysicalDeviceProperties.limits.maxInterpolationOffset << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::subPixelInterpolationOffsetBits: " << m_vPhysicalDeviceProperties.limits.subPixelInterpolationOffsetBits << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxFramebufferWidth: " << m_vPhysicalDeviceProperties.limits.maxFramebufferWidth << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxFramebufferHeight: " << m_vPhysicalDeviceProperties.limits.maxFramebufferHeight << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxFramebufferLayers: " << m_vPhysicalDeviceProperties.limits.maxFramebufferLayers << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::framebufferColorSampleCounts: " << m_vPhysicalDeviceProperties.limits.framebufferColorSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::framebufferDepthSampleCounts: " << m_vPhysicalDeviceProperties.limits.framebufferDepthSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::framebufferStencilSampleCounts: " << m_vPhysicalDeviceProperties.limits.framebufferStencilSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::framebufferNoAttachmentsSampleCounts: " << m_vPhysicalDeviceProperties.limits.framebufferNoAttachmentsSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxColorAttachments: " << m_vPhysicalDeviceProperties.limits.maxColorAttachments << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::sampledImageColorSampleCounts: " << m_vPhysicalDeviceProperties.limits.sampledImageColorSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::sampledImageIntegerSampleCounts: " << m_vPhysicalDeviceProperties.limits.sampledImageIntegerSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::sampledImageDepthSampleCounts: " << m_vPhysicalDeviceProperties.limits.sampledImageDepthSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::sampledImageStencilSampleCounts: " << m_vPhysicalDeviceProperties.limits.sampledImageStencilSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::storageImageSampleCounts: " << m_vPhysicalDeviceProperties.limits.storageImageSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxSampleMaskWords: " << m_vPhysicalDeviceProperties.limits.maxSampleMaskWords << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::timestampComputeAndGraphics: " << m_vPhysicalDeviceProperties.limits.timestampComputeAndGraphics << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::timestampPeriod: " << m_vPhysicalDeviceProperties.limits.timestampPeriod << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxClipDistances: " << m_vPhysicalDeviceProperties.limits.maxClipDistances << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxCullDistances: " << m_vPhysicalDeviceProperties.limits.maxCullDistances << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxCombinedClipAndCullDistances: " << m_vPhysicalDeviceProperties.limits.maxCombinedClipAndCullDistances << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::discreteQueuePriorities: " << m_vPhysicalDeviceProperties.limits.discreteQueuePriorities << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::pointSizeRange: " << m_vPhysicalDeviceProperties.limits.pointSizeRange << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::lineWidthRange: " << m_vPhysicalDeviceProperties.limits.lineWidthRange << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::pointSizeGranularity: " << m_vPhysicalDeviceProperties.limits.pointSizeGranularity << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::lineWidthGranularity: " << m_vPhysicalDeviceProperties.limits.lineWidthGranularity << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::strictLines: " << m_vPhysicalDeviceProperties.limits.strictLines << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::standardSampleLocations: " << m_vPhysicalDeviceProperties.limits.standardSampleLocations << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::optimalBufferCopyOffsetAlignment: " << m_vPhysicalDeviceProperties.limits.optimalBufferCopyOffsetAlignment << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::optimalBufferCopyRowPitchAlignment: " << m_vPhysicalDeviceProperties.limits.optimalBufferCopyRowPitchAlignment << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::nonCoherentAtomSize: " << m_vPhysicalDeviceProperties.limits.nonCoherentAtomSize << std::endl;

#endif	// FLINT_DEBUG
		}

		void VulkanDevice::InitializeLogicalDevice()
		{
			OPTICK_EVENT();

			m_vQueue.Initialize(m_vPhysicalDevice, m_Flags);

			std::vector<VkDeviceQueueCreateInfo> m_vQueueCreateInfos;
			std::set<uint32_t> uniqueQueueFamilies = {
				m_vQueue.m_TransferFamily.value()
			};

			if (m_vQueue.m_GraphicsFamily.has_value())
				uniqueQueueFamilies.insert(m_vQueue.m_GraphicsFamily.value());

			if (m_vQueue.m_ComputeFamily.has_value())
				uniqueQueueFamilies.insert(m_vQueue.m_ComputeFamily.value());

			float queuePriority = 1.0f;
			for (uint32_t queueFamily : uniqueQueueFamilies)
			{
				VkDeviceQueueCreateInfo m_vQueueCreateInfo = {};
				m_vQueueCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				m_vQueueCreateInfo.queueFamilyIndex = queueFamily;
				m_vQueueCreateInfo.queueCount = 1;
				m_vQueueCreateInfo.pQueuePriorities = &queuePriority;
				m_vQueueCreateInfos.emplace_back(m_vQueueCreateInfo);
			}

			// Physical device features.
			VkPhysicalDeviceFeatures m_vFeatures = {};
			m_vFeatures.samplerAnisotropy = VK_TRUE;
			m_vFeatures.sampleRateShading = VK_TRUE; // Enable sample shading feature for the device
			m_vFeatures.fillModeNonSolid = VK_TRUE;
			m_vFeatures.logicOp = VK_TRUE;
			m_vFeatures.geometryShader = VK_TRUE;
			m_vFeatures.tessellationShader = VK_TRUE;

			// Device create info.
			VkDeviceCreateInfo m_vDeviceCreateInfo = {};
			m_vDeviceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			m_vDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(m_vQueueCreateInfos.size());
			m_vDeviceCreateInfo.pQueueCreateInfos = m_vQueueCreateInfos.data();
			m_vDeviceCreateInfo.pEnabledFeatures = &m_vFeatures;
			m_vDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
			m_vDeviceCreateInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

			const std::vector<const char*> validationLayers = pInstance->GetValidationLayers();
			if (pInstance->IsValidationEnabled())
			{
				m_vDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
				m_vDeviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
			}
			else
				m_vDeviceCreateInfo.enabledLayerCount = 0;

			// Create the logical device.
			FLINT_VK_ASSERT(vkCreateDevice(m_vPhysicalDevice, &m_vDeviceCreateInfo, nullptr, &m_vLogicalDevice));
			volkLoadDeviceTable(&m_DeviceTable, m_vLogicalDevice);

			if ((m_Flags & DeviceFlags::GraphicsCompatible) == DeviceFlags::GraphicsCompatible)
				GetDeviceTable().vkGetDeviceQueue(GetLogicalDevice(), m_vQueue.m_GraphicsFamily.value(), 0, &m_vQueue.m_vGraphicsQueue);

			if ((m_Flags & DeviceFlags::ComputeCompatible) == DeviceFlags::ComputeCompatible)
				GetDeviceTable().vkGetDeviceQueue(GetLogicalDevice(), m_vQueue.m_ComputeFamily.value(), 0, &m_vQueue.m_vComputeQueue);

			GetDeviceTable().vkGetDeviceQueue(GetLogicalDevice(), m_vQueue.m_TransferFamily.value(), 0, &m_vQueue.m_vTransferQueue);

			// Create the VMA allocator.
			CreateVmaAllocator();
		}

		void VulkanDevice::TerminateLogicalDevice()
		{
			GetDeviceTable().vkDestroyDevice(m_vLogicalDevice, nullptr);
		}

		VmaVulkanFunctions VulkanDevice::GetVulkanFunctions() const
		{
			VmaVulkanFunctions functions = {};
			functions.vkAllocateMemory = GetDeviceTable().vkAllocateMemory;
			functions.vkBindBufferMemory = GetDeviceTable().vkBindBufferMemory;
			functions.vkBindBufferMemory2KHR = GetDeviceTable().vkBindBufferMemory2;
			functions.vkBindImageMemory = GetDeviceTable().vkBindImageMemory;
			functions.vkBindImageMemory2KHR = GetDeviceTable().vkBindImageMemory2;
			functions.vkCmdCopyBuffer = GetDeviceTable().vkCmdCopyBuffer;
			functions.vkCreateBuffer = GetDeviceTable().vkCreateBuffer;
			functions.vkCreateImage = GetDeviceTable().vkCreateImage;
			functions.vkDestroyBuffer = GetDeviceTable().vkDestroyBuffer;
			functions.vkDestroyImage = GetDeviceTable().vkDestroyImage;
			functions.vkFlushMappedMemoryRanges = GetDeviceTable().vkFlushMappedMemoryRanges;
			functions.vkFreeMemory = GetDeviceTable().vkFreeMemory;
			functions.vkGetBufferMemoryRequirements = GetDeviceTable().vkGetBufferMemoryRequirements;
			functions.vkGetBufferMemoryRequirements2KHR = GetDeviceTable().vkGetBufferMemoryRequirements2;
			functions.vkGetImageMemoryRequirements = GetDeviceTable().vkGetImageMemoryRequirements;
			functions.vkGetImageMemoryRequirements2KHR = GetDeviceTable().vkGetImageMemoryRequirements2;
			functions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
			functions.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2;
			functions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
			functions.vkInvalidateMappedMemoryRanges = GetDeviceTable().vkInvalidateMappedMemoryRanges;
			functions.vkMapMemory = GetDeviceTable().vkMapMemory;
			functions.vkUnmapMemory = GetDeviceTable().vkUnmapMemory;

			return functions;
		}

		void VulkanDevice::CreateVmaAllocator()
		{
			VmaAllocatorCreateInfo vmaCreateInfo = {};
			vmaCreateInfo.instance = pInstance->StaticCast<VulkanInstance>().GetInstance();
			vmaCreateInfo.physicalDevice = GetPhysicalDevice();
			vmaCreateInfo.device = GetLogicalDevice();
			vmaCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;

			const VmaVulkanFunctions functions = GetVulkanFunctions();
			vmaCreateInfo.pVulkanFunctions = &functions;

			FLINT_VK_ASSERT(vmaCreateAllocator(&vmaCreateInfo, &m_VmaAllocator));
		}

		void VulkanDevice::DestroyVmaAllocator() const
		{
			vmaDestroyAllocator(m_VmaAllocator);
		}
	}
}