// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

export module Flint.VulkanBackend.VulkanDevice;

#include "GraphicsCore/Device.hpp"

import Flint.VulkanBackend.VulkanInstance;
import Flint.VulkanBackend.VulkanQueue;

#include <vk_mem_alloc.h>

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDevice final : public Device, public std::enable_shared_from_this<VulkanDevice>
		{
		public:
			VulkanDevice(const std::shared_ptr<Instance>& pInstance, const DeviceFlags flags);
			~VulkanDevice() { if (!bIsTerminated) Terminate(); }

			virtual bool IsDisplayCompatible(const Display* pDisplay) override;
			virtual MultiSampleCount GetSupportedMultiSampleCount() const override;

			virtual std::shared_ptr<CommandBufferAllocator> CreateCommandBufferAllocator(const uint32 bufferCount) override;
			virtual std::shared_ptr<CommandBufferAllocator> CreateSecondaryCommandBufferAllocator(const uint32 bufferCount, const std::shared_ptr<CommandBufferAllocator>& pParentAllocator) override;

			virtual std::shared_ptr<SwapChain> CreateSwapChain(const std::shared_ptr<Display>& pDisplay, uint32 imageCount, const SwapChainPresentMode presentMode) override;

			virtual std::shared_ptr<ScreenBoundRenderTarget> CreateScreenBoundRenderTarget(const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const uint32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments, const SwapChainPresentMode presentMode, const FColor4D& swapChainClearColor = FColor4D(0.0f)) override;
			virtual std::shared_ptr<OffScreenRenderTarget> CreateOffScreenRenderTarget(const FBox2D& extent, const uint32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments) override;

			virtual std::shared_ptr<Buffer> CreateBuffer(const BufferType type, const uint64 size, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic) override;

			virtual std::shared_ptr<Image> CreateImage(const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const uint8 layers, const uint32 mipLevels, const void* pImageData, const MultiSampleCount sampleCount = MultiSampleCount::One) override;

			virtual std::shared_ptr<ImageSampler> CreateImageSampler(const ImageSamplerSpecification& specification) override;

			virtual std::shared_ptr<Shader> CreateShader(const ShaderType type, const std::filesystem::path& path) override;
			virtual std::shared_ptr<Shader> CreateShader(const ShaderType type, const std::vector<uint32>& code) override;
			virtual std::shared_ptr<Shader> CreateShader(const ShaderType type, const std::string& code) override;

			virtual std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(const std::string& pipelineName, const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget, const std::shared_ptr<Shader>& pVertexShader, const std::shared_ptr<Shader>& pTessellationControlShader, const std::shared_ptr<Shader>& pTessellationEvaluationShader, const std::shared_ptr<Shader>& pGeometryShader, const std::shared_ptr<Shader>& pFragmentShader, const GraphicsPipelineSpecification& specification) override;
			virtual std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(const std::string& pipelineName, const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget, const std::shared_ptr<Shader>& pVertexShader, const std::shared_ptr<Shader>& pTessellationControlShader, const std::shared_ptr<Shader>& pTessellationEvaluationShader, const std::shared_ptr<Shader>& pGeometryShader, const std::shared_ptr<Shader>& pFragmentShader, const GraphicsPipelineSpecification& specification) override;
			virtual std::shared_ptr<ComputePipeline> CreateComputePipeline(const std::string& pipelineName, const std::shared_ptr<Shader>& pShader) override;

			virtual std::shared_ptr<GeometryStore> CreateGeometryStore(const typename std::vector<ShaderAttribute>& vertexAttributes, uint64 indexSize, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic) override;

			virtual std::shared_ptr<HostSynchronizationPrimitive> CreateHostSynchronizationPrimitive() override;
			virtual std::shared_ptr<DeviceSynchronizationPrimitive> CreateDeviceSynchronizationPrimitive() override;
			virtual std::shared_ptr<Query> CreateQuery(const QueryUsage usage, const uint32 queryCount) override;

			virtual void SubmitGraphicsCommandBuffer(const CommandBuffer* pCommandBuffer, SynchronizationPrimitive* pPrimitive = nullptr) override;
			virtual void SubmitGraphicsCommandBuffers(const std::vector<CommandBuffer*>& pCommandBuffers, SynchronizationPrimitive* pPrimitive = nullptr) override;
			virtual void SubmitComputeCommandBuffer(const CommandBuffer* pCommandBuffer, SynchronizationPrimitive* pPrimitive = nullptr) override;
			virtual void SubmitComputeCommandBuffers(const std::vector<CommandBuffer*>& pCommandBuffers, SynchronizationPrimitive* pPrimitive = nullptr) override;
			virtual void WaitIdle() override;
			virtual void WaitForQueue() override;

			virtual void Terminate() override;

		public:
			VolkDeviceTable GetDeviceTable() const { return mDeviceTable; }
			VkDevice GetLogicalDevice() const noexcept { return vLogicalDevice; }
			VkPhysicalDevice GetPhysicalDevice() const noexcept { return vPhysicalDevice; }

			VulkanQueue& GetQueue() { return vQueue; }
			const VulkanQueue GetQueue() const { return vQueue; }
			VkSampleCountFlags GetSampleCount() const { return vSampleCount; }

			VkResult CreateImageMemory(const std::vector<VkImage>& vImages, VkMemoryPropertyFlags vMemoryflags, VkDeviceMemory* pDeviceMemory) const;
			void SetImageLayout(VkCommandBuffer vCommandBuffer, VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, uint32 layerCount = 1, uint32 currentLayer = 0, const uint32 mipLevels = 1, const uint32 currentLevel = 0, VkImageAspectFlags vAspects = VkImageAspectFlagBits::VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM) const;
			void SetImageLayout(VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, uint32 layerCount = 1, uint32 currentLayer = 0, const uint32 mipLevels = 1, const uint32 currentLevel = 0, VkImageAspectFlags vAspects = VkImageAspectFlagBits::VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM) const;

			const VmaAllocator GetVmaAllocator() const { return mVmaAllocator; }

		private:
			void InitializePhysicalDevice();

			void InitializeLogicalDevice();
			void TerminateLogicalDevice();

			VmaVulkanFunctions GetVulkanFunctions() const;

			void CreateVmaAllocator();
			void DestroyVmaAllocator() const;

		private:
			VolkDeviceTable mDeviceTable = {};
			VulkanQueue vQueue = {};

			std::vector<const char*> mDeviceExtensions;

			VkDevice vLogicalDevice = VK_NULL_HANDLE;
			VkPhysicalDevice vPhysicalDevice = VK_NULL_HANDLE;

			VmaAllocator mVmaAllocator = VK_NULL_HANDLE;

			VkSampleCountFlags vSampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT;
		};
	}
}

module: private;

import Flint.VulkanBackend.VulkanDisplay;
import Flint.VulkanBackend.VulkanOneTimeCommandBuffer;
import Flint.VulkanBackend.VulkanCommandBuffer;
import Flint.VulkanBackend.VulkanCommandBufferAllocator;
import Flint.VulkanBackend.VulkanSwapChain;
import Flint.VulkanBackend.VulkanScreenBoundRenderTarget;
import Flint.VulkanBackend.VulkanOffScreenRenderTarget;
import Flint.VulkanBackend.VulkanBuffer;
import Flint.VulkanBackend.VulkanImage;
import Flint.VulkanBackend.VulkanImageSampler;
import Flint.VulkanBackend.VulkanShader;
import Flint.VulkanBackend.VulkanGraphicsPipeline;
import Flint.VulkanBackend.VulkanComputePipeline;
import Flint.VulkanBackend.VulkanHostSynchronizationPrimitive;
import Flint.VulkanBackend.VulkanDeviceSynchronizationPrimitive;
import Flint.VulkanBackend.VulkanQuery;

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

			bool CheckDeviceExtensionSupport(VkPhysicalDevice vPhysicalDevice, std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME })
			{
				uint32 extensionCount = 0;
				vkEnumerateDeviceExtensionProperties(vPhysicalDevice, nullptr, &extensionCount, nullptr);

				std::vector<VkExtensionProperties> availableExtensions(extensionCount);
				vkEnumerateDeviceExtensionProperties(vPhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

				std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

				for (const VkExtensionProperties& extension : availableExtensions)
					requiredExtensions.erase(extension.extensionName);

				return requiredExtensions.empty();
			}

			bool IsPhysicalDeviceSuitable(VkPhysicalDevice vDevice, const std::vector<const char*>& deviceExtensions, const DeviceFlags flags)
			{
				VulkanQueue vQueue = {};
				vQueue.Initialize(vDevice, flags);

				const bool extensionsSupported = Helpers::CheckDeviceExtensionSupport(vDevice, deviceExtensions);

				VkPhysicalDeviceFeatures supportedFeatures = {};
				vkGetPhysicalDeviceFeatures(vDevice, &supportedFeatures);

				return vQueue.IsComplete()
					&& extensionsSupported
					&& supportedFeatures.samplerAnisotropy;
			}
		}

		VulkanDevice::VulkanDevice(const std::shared_ptr<Instance>& pInstance, const DeviceFlags flags) : Device(pInstance, flags)
		{
			OPTICK_EVENT();

			if ((flags & DeviceFlags::GraphicsCompatible) == DeviceFlags::GraphicsCompatible)
				mDeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

			InitializePhysicalDevice();
			InitializeLogicalDevice();
		}

		bool VulkanDevice::IsDisplayCompatible(const Display* pDisplay)
		{
			OPTICK_EVENT();

			const VulkanDisplay& vDisplay = pDisplay->StaticCast<VulkanDisplay>();
			VkBool32 isSupported = VK_FALSE;
			FLINT_VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(GetPhysicalDevice(), GetQueue().mGraphicsFamily.value(), vDisplay.GetSurface(), &isSupported));
			return isSupported == VK_TRUE;
		}

		MultiSampleCount VulkanDevice::GetSupportedMultiSampleCount() const
		{
			return MultiSampleCount(vSampleCount);
		}

		std::shared_ptr<CommandBufferAllocator> VulkanDevice::CreateCommandBufferAllocator(const uint32 bufferCount)
		{
			return std::make_shared<VulkanCommandBufferAllocator>(shared_from_this(), bufferCount);
		}

		std::shared_ptr<CommandBufferAllocator> VulkanDevice::CreateSecondaryCommandBufferAllocator(const uint32 bufferCount, const std::shared_ptr<CommandBufferAllocator>& pParentAllocator)
		{
			return std::make_shared<VulkanCommandBufferAllocator>(shared_from_this(), pParentAllocator, bufferCount);
		}

		std::shared_ptr<SwapChain> VulkanDevice::CreateSwapChain(const std::shared_ptr<Display>& pDisplay, uint32 imageCount, const SwapChainPresentMode presentMode)
		{
			return std::make_shared<VulkanSwapChain>(shared_from_this(), pDisplay, imageCount, presentMode);
		}

		std::shared_ptr<ScreenBoundRenderTarget> VulkanDevice::CreateScreenBoundRenderTarget(const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const uint32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments, const SwapChainPresentMode presentMode, const FColor4D& swapChainClearColor)
		{
			return  std::make_shared<VulkanScreenBoundRenderTarget>(shared_from_this(), pDisplay, extent, bufferCount, imageAttachments, presentMode, swapChainClearColor);
		}

		std::shared_ptr<OffScreenRenderTarget> VulkanDevice::CreateOffScreenRenderTarget(const FBox2D& extent, const uint32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments)
		{
			return std::make_shared<VulkanOffScreenRenderTarget>(shared_from_this(), extent, bufferCount, imageAttachments);
		}

		std::shared_ptr<Buffer> VulkanDevice::CreateBuffer(const BufferType type, const uint64 size, const BufferMemoryProfile profile)
		{
			return std::make_shared<VulkanBuffer>(shared_from_this(), type, size, profile);
		}

		std::shared_ptr<Image> VulkanDevice::CreateImage(const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const uint8 layers, const uint32 mipLevels, const void* pImageData, const MultiSampleCount sampleCount)
		{
			return std::make_shared<VulkanImage>(shared_from_this(), type, usage, extent, format, layers, mipLevels, pImageData, sampleCount);
		}

		std::shared_ptr<ImageSampler> VulkanDevice::CreateImageSampler(const ImageSamplerSpecification& specification)
		{
			return std::make_shared<VulkanImageSampler>(shared_from_this(), specification);
		}

		std::shared_ptr<Shader> VulkanDevice::CreateShader(const ShaderType type, const std::filesystem::path& path)
		{
			return std::make_shared<VulkanShader>(shared_from_this(), type, path);
		}

		std::shared_ptr<Shader> VulkanDevice::CreateShader(const ShaderType type, const std::vector<uint32>& code)
		{
			return std::make_shared<VulkanShader>(shared_from_this(), type, code);
		}

		std::shared_ptr<Shader> VulkanDevice::CreateShader(const ShaderType type, const std::string& code)
		{
			return std::make_shared<VulkanShader>(shared_from_this(), type, code);
		}

		std::shared_ptr<GraphicsPipeline> VulkanDevice::CreateGraphicsPipeline(const std::string& pipelineName, const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget, const std::shared_ptr<Shader>& pVertexShader, const std::shared_ptr<Shader>& pTessellationControlShader, const std::shared_ptr<Shader>& pTessellationEvaluationShader, const std::shared_ptr<Shader>& pGeometryShader, const std::shared_ptr<Shader>& pFragmentShader, const GraphicsPipelineSpecification& specification)
		{
			return std::make_shared<VulkanGraphicsPipeline>(shared_from_this(), pipelineName, pScreenBoundRenderTarget, pVertexShader, pTessellationControlShader, pTessellationEvaluationShader, pGeometryShader, pFragmentShader, specification);
		}

		std::shared_ptr<GraphicsPipeline> VulkanDevice::CreateGraphicsPipeline(const std::string& pipelineName, const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget, const std::shared_ptr<Shader>& pVertexShader, const std::shared_ptr<Shader>& pTessellationControlShader, const std::shared_ptr<Shader>& pTessellationEvaluationShader, const std::shared_ptr<Shader>& pGeometryShader, const std::shared_ptr<Shader>& pFragmentShader, const GraphicsPipelineSpecification& specification)
		{
			return std::make_shared<VulkanGraphicsPipeline>(shared_from_this(), pipelineName, pOffScreenRenderTarget, pVertexShader, pTessellationControlShader, pTessellationEvaluationShader, pGeometryShader, pFragmentShader, specification);
		}

		std::shared_ptr<ComputePipeline> VulkanDevice::CreateComputePipeline(const std::string& pipelineName, const std::shared_ptr<Shader>& pShader)
		{
			return std::make_shared<VulkanComputePipeline>(shared_from_this(), pipelineName, pShader);
		}

		std::shared_ptr<GeometryStore> VulkanDevice::CreateGeometryStore(const std::vector<ShaderAttribute>& vertexAttributes, uint64 indexSize, const BufferMemoryProfile profile)
		{
			return std::make_shared<GeometryStore>(shared_from_this(), vertexAttributes, indexSize, profile);
		}

		std::shared_ptr<HostSynchronizationPrimitive> VulkanDevice::CreateHostSynchronizationPrimitive()
		{
			return std::make_shared<VulkanHostSynchronizationPrimitive>(shared_from_this());
		}

		std::shared_ptr<DeviceSynchronizationPrimitive> VulkanDevice::CreateDeviceSynchronizationPrimitive()
		{
			return std::make_shared<VulkanDeviceSynchronizationPrimitive>(shared_from_this());
		}

		std::shared_ptr<Query> VulkanDevice::CreateQuery(const QueryUsage usage, const uint32 queryCount)
		{
			return std::make_shared<VulkanQuery>(shared_from_this(), usage, queryCount);
		}

		void VulkanDevice::SubmitGraphicsCommandBuffer(const CommandBuffer* pCommandBuffer, SynchronizationPrimitive* pPrimitive)
		{
			OPTICK_EVENT();
			auto& vCommandBuffer = pCommandBuffer->StaticCast<VulkanCommandBuffer>();

			VkFence vFence = VK_NULL_HANDLE;
			if (pPrimitive)
			{
				VulkanHostSynchronizationPrimitive& vPrimitive = pPrimitive->StaticCast<VulkanHostSynchronizationPrimitive>();
				vPrimitive.Reset();
				vFence = vPrimitive.GetFence();
			}

			FLINT_VK_ASSERT(GetDeviceTable().vkQueueSubmit(GetQueue().vGraphicsQueue, 1, vCommandBuffer.GetSubmitInfoAddress(), vFence));
		}

		void VulkanDevice::SubmitGraphicsCommandBuffers(const std::vector<CommandBuffer*>& pCommandBuffers, SynchronizationPrimitive* pPrimitive)
		{
			OPTICK_EVENT();
			const uint64 bufferCount = pCommandBuffers.size();

			std::vector<VkSubmitInfo> vSubmitInfos(bufferCount);
			for (uint64 i = 0; i < bufferCount; i++)
			{
				auto& vCommandBuffer = pCommandBuffers[i]->StaticCast<VulkanCommandBuffer>();
				vSubmitInfos[i] = vCommandBuffer.GetSubmitInfo();
			}

			VkFence vFence = VK_NULL_HANDLE;
			if (pPrimitive)
			{
				VulkanHostSynchronizationPrimitive& vPrimitive = pPrimitive->StaticCast<VulkanHostSynchronizationPrimitive>();
				vPrimitive.Reset();
				vFence = vPrimitive.GetFence();
			}

			FLINT_VK_ASSERT(GetDeviceTable().vkQueueSubmit(GetQueue().vGraphicsQueue, static_cast<uint32>(vSubmitInfos.size()), vSubmitInfos.data(), vFence));
		}

		void VulkanDevice::SubmitComputeCommandBuffer(const CommandBuffer* pCommandBuffer, SynchronizationPrimitive* pPrimitive)
		{
			OPTICK_EVENT();
			auto& vCommandBuffer = pCommandBuffer->StaticCast<VulkanCommandBuffer>();

			VkFence vFence = VK_NULL_HANDLE;
			if (pPrimitive)
			{
				VulkanHostSynchronizationPrimitive& vPrimitive = pPrimitive->StaticCast<VulkanHostSynchronizationPrimitive>();
				vPrimitive.Reset();
				vFence = vPrimitive.GetFence();
			}

			FLINT_VK_ASSERT(GetDeviceTable().vkQueueSubmit(GetQueue().vGraphicsQueue, 1, vCommandBuffer.GetSubmitInfoAddress(), vFence));
		}

		void VulkanDevice::SubmitComputeCommandBuffers(const std::vector<CommandBuffer*>& pCommandBuffers, SynchronizationPrimitive* pPrimitive)
		{
			OPTICK_EVENT();
			const uint64 bufferCount = pCommandBuffers.size();

			std::vector<VkSubmitInfo> vSubmitInfos(bufferCount);
			for (uint64 i = 0; i < bufferCount; i++)
			{
				auto& vCommandBuffer = pCommandBuffers[i]->StaticCast<VulkanCommandBuffer>();
				vSubmitInfos[i] = vCommandBuffer.GetSubmitInfo();
			}

			VkFence vFence = VK_NULL_HANDLE;
			if (pPrimitive)
			{
				VulkanHostSynchronizationPrimitive& vPrimitive = pPrimitive->StaticCast<VulkanHostSynchronizationPrimitive>();
				vPrimitive.Reset();
				vFence = vPrimitive.GetFence();
			}

			FLINT_VK_ASSERT(GetDeviceTable().vkQueueSubmit(GetQueue().vComputeQueue, static_cast<uint32>(vSubmitInfos.size()), vSubmitInfos.data(), vFence));
		}

		void VulkanDevice::WaitIdle()
		{
			OPTICK_EVENT();

			FLINT_VK_ASSERT(GetDeviceTable().vkDeviceWaitIdle(GetLogicalDevice()));
		}

		void VulkanDevice::WaitForQueue()
		{
			OPTICK_EVENT();

			FLINT_VK_ASSERT(GetDeviceTable().vkQueueWaitIdle(GetQueue().vTransferQueue));
		}

		void VulkanDevice::Terminate()
		{
			TerminateLogicalDevice();

			// Destroy the VMA allocator.
			DestroyVmaAllocator();

			bIsTerminated = true;
		}

		VkResult VulkanDevice::CreateImageMemory(const std::vector<VkImage>& vImages, VkMemoryPropertyFlags vMemoryFlags, VkDeviceMemory* pDeviceMemory) const
		{
			OPTICK_EVENT();

			if (vImages.empty())
				return VkResult::VK_ERROR_UNKNOWN;

			VkMemoryRequirements vMR = {};
			GetDeviceTable().vkGetImageMemoryRequirements(GetLogicalDevice(), vImages[0], &vMR);

			VkPhysicalDeviceMemoryProperties vMP = {};
			vkGetPhysicalDeviceMemoryProperties(GetPhysicalDevice(), &vMP);

			VkMemoryAllocateInfo vAI = {};
			vAI.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			vAI.allocationSize = vMR.size * vImages.size();

			for (uint32 i = 0; i < vMP.memoryTypeCount; i++)
			{
				if ((vMR.memoryTypeBits & (1 << i)) && (vMP.memoryTypes[i].propertyFlags & vMemoryFlags) == vMemoryFlags)
				{
					vAI.memoryTypeIndex = i;
					break;
				}
			}

			FLINT_VK_ASSERT(GetDeviceTable().vkAllocateMemory(GetLogicalDevice(), &vAI, nullptr, pDeviceMemory));

			VkResult result = VkResult::VK_ERROR_UNKNOWN;
			for (uint32 i = 0; i < vImages.size(); i++)
				result = GetDeviceTable().vkBindImageMemory(GetLogicalDevice(), vImages[i], *pDeviceMemory, vMR.size * i);

			return result;
		}

		void VulkanDevice::SetImageLayout(VkCommandBuffer vCommandBuffer, VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, uint32 layerCount, uint32 currentLayer, const uint32 mipLevels, const uint32 currentLevel, VkImageAspectFlags vAspects) const
		{
			OPTICK_EVENT();

			VkImageMemoryBarrier vMB = {};
			vMB.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			vMB.oldLayout = vOldLayout;
			vMB.newLayout = vNewLayout;
			vMB.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			vMB.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			vMB.image = vImage;

			vMB.subresourceRange.aspectMask = vAspects;
			if (vAspects == VkImageAspectFlagBits::VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM)
			{
				if (vOldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
					vOldLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL ||
					vOldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL ||
					vNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
					vNewLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL ||
					vNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)
				{
					vMB.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

					if (Utilities::HasStencilComponent(vFormat))
						vMB.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
				}
				else
					vMB.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			}
			else
			{
				if (Utilities::HasStencilComponent(vFormat))
					vMB.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}

			vMB.subresourceRange.baseMipLevel = currentLevel;
			vMB.subresourceRange.levelCount = mipLevels;
			vMB.subresourceRange.layerCount = layerCount;
			vMB.subresourceRange.baseArrayLayer = currentLayer;
			vMB.srcAccessMask = 0;
			vMB.dstAccessMask = 0;

			switch (vOldLayout)
			{
			case VK_IMAGE_LAYOUT_GENERAL:
			case VK_IMAGE_LAYOUT_UNDEFINED:
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

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
				vMB.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				vMB.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
				//vMB.srcAccessMask = VK_ACCESS_;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				vMB.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				vMB.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;

			default:
				throw std::runtime_error("Unsupported layout transition!");
			}

			switch (vNewLayout)
			{
			case VK_IMAGE_LAYOUT_UNDEFINED:
			case VK_IMAGE_LAYOUT_GENERAL:
			case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				vMB.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				vMB.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				vMB.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				vMB.dstAccessMask = vMB.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				vMB.srcAccessMask |= VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
				vMB.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;

			default:
				throw std::runtime_error("Unsupported layout transition!");
			}

			const VkPipelineStageFlags sourceStage = Utilities::GetPipelineStageFlags(vMB.srcAccessMask);
			const VkPipelineStageFlags destinationStage = Utilities::GetPipelineStageFlags(vMB.dstAccessMask);
			GetDeviceTable().vkCmdPipelineBarrier(vCommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &vMB);
		}

		void VulkanDevice::SetImageLayout(VkImage vImage, VkImageLayout vOldLayout, VkImageLayout vNewLayout, VkFormat vFormat, uint32 layerCount, uint32 currentLayer, const uint32 mipLevels, const uint32 currentLevel, VkImageAspectFlags vAspects) const
		{
			OPTICK_EVENT();

			const VulkanOneTimeCommandBuffer vCommandBuffer(*this);
			SetImageLayout(vCommandBuffer, vImage, vOldLayout, vNewLayout, vFormat, layerCount, currentLayer, mipLevels, currentLevel, vAspects);
		}

		void VulkanDevice::InitializePhysicalDevice()
		{
			OPTICK_EVENT();

			const auto& vInstance = pInstance->StaticCast<VulkanInstance>();

			uint32 deviceCount = 0;
			vkEnumeratePhysicalDevices(vInstance.GetInstance(), &deviceCount, nullptr);

			if (deviceCount == 0)
				throw std::runtime_error("No available devices found!");

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(vInstance.GetInstance(), &deviceCount, devices.data());

			VkPhysicalDeviceProperties vPhysicalDeviceProperties = {};
			// Iterate through all the candidate devices and find the best device.
			for (const VkPhysicalDevice& device : devices)
			{
				if (Helpers::IsPhysicalDeviceSuitable(device, mDeviceExtensions, mFlags))
				{
					vkGetPhysicalDeviceProperties(device, &vPhysicalDeviceProperties);

					if (vPhysicalDeviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && (mFlags & DeviceFlags::External) == DeviceFlags::External)
						vPhysicalDevice = device;
					else if (vPhysicalDeviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU && (mFlags & DeviceFlags::Integrated) == DeviceFlags::Integrated)
						vPhysicalDevice = device;
					else
						vPhysicalDevice = device;

					break;
				}

				vPhysicalDevice = VK_NULL_HANDLE;
			}

			//  Check if a physical device was found.
			if (vPhysicalDevice == VK_NULL_HANDLE)
				throw std::runtime_error("Unable to find suitable physical device!");

			vSampleCount = Helpers::GetMaxUsableSamples(vPhysicalDevice);

#ifdef FLINT_DEBUG
			std::cout << "Vulkan Physical Device Information" << std::endl;
			std::cout << "\tAPI Version: " << vPhysicalDeviceProperties.apiVersion << std::endl;
			std::cout << "\tDriver Version: " << vPhysicalDeviceProperties.driverVersion << std::endl;
			std::cout << "\tVendor ID: " << vPhysicalDeviceProperties.vendorID << std::endl;
			std::cout << "\tDevice ID: " << vPhysicalDeviceProperties.deviceID << std::endl;
			std::cout << "\tDevice Name: " << vPhysicalDeviceProperties.deviceName << std::endl;

			switch (vPhysicalDeviceProperties.deviceType)
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
			for (const auto number : vPhysicalDeviceProperties.pipelineCacheUUID)
				std::cout << static_cast<uint32>(number);
			std::cout << std::endl;

			std::cout << "\tDevice limits:" << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxImageDimension1D: " << vPhysicalDeviceProperties.limits.maxImageDimension1D << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxImageDimension2D: " << vPhysicalDeviceProperties.limits.maxImageDimension2D << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxImageDimension3D: " << vPhysicalDeviceProperties.limits.maxImageDimension3D << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxImageDimensionCube: " << vPhysicalDeviceProperties.limits.maxImageDimensionCube << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxImageArrayLayers: " << vPhysicalDeviceProperties.limits.maxImageArrayLayers << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTexelBufferElements: " << vPhysicalDeviceProperties.limits.maxTexelBufferElements << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxUniformBufferRange: " << vPhysicalDeviceProperties.limits.maxUniformBufferRange << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxStorageBufferRange: " << vPhysicalDeviceProperties.limits.maxStorageBufferRange << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxPushConstantsSize: " << vPhysicalDeviceProperties.limits.maxPushConstantsSize << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxMemoryAllocationCount: " << vPhysicalDeviceProperties.limits.maxMemoryAllocationCount << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxSamplerAllocationCount: " << vPhysicalDeviceProperties.limits.maxSamplerAllocationCount << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::bufferImageGranularity: " << vPhysicalDeviceProperties.limits.bufferImageGranularity << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::sparseAddressSpaceSize: " << vPhysicalDeviceProperties.limits.sparseAddressSpaceSize << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxBoundDescriptorSets: " << vPhysicalDeviceProperties.limits.maxBoundDescriptorSets << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxPerStageDescriptorSamplers: " << vPhysicalDeviceProperties.limits.maxPerStageDescriptorSamplers << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxPerStageDescriptorUniformBuffers: " << vPhysicalDeviceProperties.limits.maxPerStageDescriptorUniformBuffers << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxPerStageDescriptorStorageBuffers: " << vPhysicalDeviceProperties.limits.maxPerStageDescriptorStorageBuffers << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxPerStageDescriptorSampledImages: " << vPhysicalDeviceProperties.limits.maxPerStageDescriptorSampledImages << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxPerStageDescriptorStorageImages: " << vPhysicalDeviceProperties.limits.maxPerStageDescriptorStorageImages << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxPerStageDescriptorInputAttachments: " << vPhysicalDeviceProperties.limits.maxPerStageDescriptorInputAttachments << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxPerStageResources: " << vPhysicalDeviceProperties.limits.maxPerStageResources << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDescriptorSetSamplers: " << vPhysicalDeviceProperties.limits.maxDescriptorSetSamplers << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDescriptorSetUniformBuffers: " << vPhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffers << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDescriptorSetUniformBuffersDynamic: " << vPhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffersDynamic << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDescriptorSetStorageBuffers: " << vPhysicalDeviceProperties.limits.maxDescriptorSetStorageBuffers << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDescriptorSetStorageBuffersDynamic: " << vPhysicalDeviceProperties.limits.maxDescriptorSetStorageBuffersDynamic << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDescriptorSetSampledImages: " << vPhysicalDeviceProperties.limits.maxDescriptorSetSampledImages << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDescriptorSetStorageImages: " << vPhysicalDeviceProperties.limits.maxDescriptorSetStorageImages << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDescriptorSetInputAttachments: " << vPhysicalDeviceProperties.limits.maxDescriptorSetInputAttachments << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxVertexInputAttributes: " << vPhysicalDeviceProperties.limits.maxVertexInputAttributes << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxVertexInputBindings: " << vPhysicalDeviceProperties.limits.maxVertexInputBindings << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxVertexInputAttributeOffset: " << vPhysicalDeviceProperties.limits.maxVertexInputAttributeOffset << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxVertexInputBindingStride: " << vPhysicalDeviceProperties.limits.maxVertexInputBindingStride << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxVertexOutputComponents: " << vPhysicalDeviceProperties.limits.maxVertexOutputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTessellationGenerationLevel: " << vPhysicalDeviceProperties.limits.maxTessellationGenerationLevel << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTessellationPatchSize: " << vPhysicalDeviceProperties.limits.maxTessellationPatchSize << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTessellationControlPerVertexInputComponents: " << vPhysicalDeviceProperties.limits.maxTessellationControlPerVertexInputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTessellationControlPerVertexOutputComponents: " << vPhysicalDeviceProperties.limits.maxTessellationControlPerVertexOutputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTessellationControlPerPatchOutputComponents: " << vPhysicalDeviceProperties.limits.maxTessellationControlPerPatchOutputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTessellationControlTotalOutputComponents: " << vPhysicalDeviceProperties.limits.maxTessellationControlTotalOutputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTessellationEvaluationInputComponents: " << vPhysicalDeviceProperties.limits.maxTessellationEvaluationInputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTessellationEvaluationOutputComponents: " << vPhysicalDeviceProperties.limits.maxTessellationEvaluationOutputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxGeometryShaderInvocations: " << vPhysicalDeviceProperties.limits.maxGeometryShaderInvocations << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxGeometryInputComponents: " << vPhysicalDeviceProperties.limits.maxGeometryInputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxGeometryOutputComponents: " << vPhysicalDeviceProperties.limits.maxGeometryOutputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxGeometryOutputVertices: " << vPhysicalDeviceProperties.limits.maxGeometryOutputVertices << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxGeometryTotalOutputComponents: " << vPhysicalDeviceProperties.limits.maxGeometryTotalOutputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxFragmentInputComponents: " << vPhysicalDeviceProperties.limits.maxFragmentInputComponents << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxFragmentOutputAttachments: " << vPhysicalDeviceProperties.limits.maxFragmentOutputAttachments << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxFragmentDualSrcAttachments: " << vPhysicalDeviceProperties.limits.maxFragmentDualSrcAttachments << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxFragmentCombinedOutputResources: " << vPhysicalDeviceProperties.limits.maxFragmentCombinedOutputResources << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxComputeSharedMemorySize: " << vPhysicalDeviceProperties.limits.maxComputeSharedMemorySize << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxComputeWorkGroupCount: " << vPhysicalDeviceProperties.limits.maxComputeWorkGroupCount << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxComputeWorkGroupInvocations: " << vPhysicalDeviceProperties.limits.maxComputeWorkGroupInvocations << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxComputeWorkGroupSize: " << vPhysicalDeviceProperties.limits.maxComputeWorkGroupSize << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::subPixelPrecisionBits: " << vPhysicalDeviceProperties.limits.subPixelPrecisionBits << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::subTexelPrecisionBits: " << vPhysicalDeviceProperties.limits.subTexelPrecisionBits << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::mipmapPrecisionBits: " << vPhysicalDeviceProperties.limits.mipmapPrecisionBits << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDrawIndexedIndexValue: " << vPhysicalDeviceProperties.limits.maxDrawIndexedIndexValue << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxDrawIndirectCount: " << vPhysicalDeviceProperties.limits.maxDrawIndirectCount << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxSamplerLodBias: " << vPhysicalDeviceProperties.limits.maxSamplerLodBias << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxSamplerAnisotropy: " << vPhysicalDeviceProperties.limits.maxSamplerAnisotropy << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxViewports: " << vPhysicalDeviceProperties.limits.maxViewports << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxViewportDimensions: " << vPhysicalDeviceProperties.limits.maxViewportDimensions << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::viewportBoundsRange: " << vPhysicalDeviceProperties.limits.viewportBoundsRange << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::viewportSubPixelBits: " << vPhysicalDeviceProperties.limits.viewportSubPixelBits << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::minMemoryMapAlignment: " << vPhysicalDeviceProperties.limits.minMemoryMapAlignment << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::minTexelBufferOffsetAlignment: " << vPhysicalDeviceProperties.limits.minTexelBufferOffsetAlignment << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::minUniformBufferOffsetAlignment: " << vPhysicalDeviceProperties.limits.minUniformBufferOffsetAlignment << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::minStorageBufferOffsetAlignment: " << vPhysicalDeviceProperties.limits.minStorageBufferOffsetAlignment << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::minTexelOffset: " << vPhysicalDeviceProperties.limits.minTexelOffset << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTexelOffset: " << vPhysicalDeviceProperties.limits.maxTexelOffset << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::minTexelGatherOffset: " << vPhysicalDeviceProperties.limits.minTexelGatherOffset << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxTexelGatherOffset: " << vPhysicalDeviceProperties.limits.maxTexelGatherOffset << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::minInterpolationOffset: " << vPhysicalDeviceProperties.limits.minInterpolationOffset << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxInterpolationOffset: " << vPhysicalDeviceProperties.limits.maxInterpolationOffset << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::subPixelInterpolationOffsetBits: " << vPhysicalDeviceProperties.limits.subPixelInterpolationOffsetBits << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxFramebufferWidth: " << vPhysicalDeviceProperties.limits.maxFramebufferWidth << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxFramebufferHeight: " << vPhysicalDeviceProperties.limits.maxFramebufferHeight << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxFramebufferLayers: " << vPhysicalDeviceProperties.limits.maxFramebufferLayers << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::framebufferColorSampleCounts: " << vPhysicalDeviceProperties.limits.framebufferColorSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::framebufferDepthSampleCounts: " << vPhysicalDeviceProperties.limits.framebufferDepthSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::framebufferStencilSampleCounts: " << vPhysicalDeviceProperties.limits.framebufferStencilSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::framebufferNoAttachmentsSampleCounts: " << vPhysicalDeviceProperties.limits.framebufferNoAttachmentsSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxColorAttachments: " << vPhysicalDeviceProperties.limits.maxColorAttachments << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::sampledImageColorSampleCounts: " << vPhysicalDeviceProperties.limits.sampledImageColorSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::sampledImageIntegerSampleCounts: " << vPhysicalDeviceProperties.limits.sampledImageIntegerSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::sampledImageDepthSampleCounts: " << vPhysicalDeviceProperties.limits.sampledImageDepthSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::sampledImageStencilSampleCounts: " << vPhysicalDeviceProperties.limits.sampledImageStencilSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::storageImageSampleCounts: " << vPhysicalDeviceProperties.limits.storageImageSampleCounts << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxSampleMaskWords: " << vPhysicalDeviceProperties.limits.maxSampleMaskWords << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::timestampComputeAndGraphics: " << vPhysicalDeviceProperties.limits.timestampComputeAndGraphics << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::timestampPeriod: " << vPhysicalDeviceProperties.limits.timestampPeriod << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxClipDistances: " << vPhysicalDeviceProperties.limits.maxClipDistances << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxCullDistances: " << vPhysicalDeviceProperties.limits.maxCullDistances << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::maxCombinedClipAndCullDistances: " << vPhysicalDeviceProperties.limits.maxCombinedClipAndCullDistances << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::discreteQueuePriorities: " << vPhysicalDeviceProperties.limits.discreteQueuePriorities << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::pointSizeRange: " << vPhysicalDeviceProperties.limits.pointSizeRange << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::lineWidthRange: " << vPhysicalDeviceProperties.limits.lineWidthRange << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::pointSizeGranularity: " << vPhysicalDeviceProperties.limits.pointSizeGranularity << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::lineWidthGranularity: " << vPhysicalDeviceProperties.limits.lineWidthGranularity << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::strictLines: " << vPhysicalDeviceProperties.limits.strictLines << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::standardSampleLocations: " << vPhysicalDeviceProperties.limits.standardSampleLocations << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::optimalBufferCopyOffsetAlignment: " << vPhysicalDeviceProperties.limits.optimalBufferCopyOffsetAlignment << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::optimalBufferCopyRowPitchAlignment: " << vPhysicalDeviceProperties.limits.optimalBufferCopyRowPitchAlignment << std::endl;
			std::cout << "\t\tVkPhysicalDeviceProperties::limits::nonCoherentAtomSize: " << vPhysicalDeviceProperties.limits.nonCoherentAtomSize << std::endl;

#endif	// FLINT_DEBUG
		}

		void VulkanDevice::InitializeLogicalDevice()
		{
			OPTICK_EVENT();

			const auto& vInstance = pInstance->StaticCast<VulkanInstance>();

			vQueue.Initialize(vPhysicalDevice, mFlags);

			std::vector<VkDeviceQueueCreateInfo> vQueueCreateInfos;
			std::set<uint32> uniqueQueueFamilies = {
				vQueue.mTransferFamily.value()
			};

			if (vQueue.mGraphicsFamily.has_value())
				uniqueQueueFamilies.insert(vQueue.mGraphicsFamily.value());

			if (vQueue.mComputeFamily.has_value())
				uniqueQueueFamilies.insert(vQueue.mComputeFamily.value());

			float queuePriority = 1.0f;
			for (uint32 queueFamily : uniqueQueueFamilies)
			{
				VkDeviceQueueCreateInfo vQueueCreateInfo = {};
				vQueueCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				vQueueCreateInfo.queueFamilyIndex = queueFamily;
				vQueueCreateInfo.queueCount = 1;
				vQueueCreateInfo.pQueuePriorities = &queuePriority;
				vQueueCreateInfos.emplace_back(vQueueCreateInfo);
			}

			// Physical device features.
			VkPhysicalDeviceFeatures vFeatures = {};
			vFeatures.samplerAnisotropy = VK_TRUE;
			vFeatures.sampleRateShading = VK_TRUE; // Enable sample shading feature for the device
			vFeatures.fillModeNonSolid = VK_TRUE;
			vFeatures.logicOp = VK_TRUE;
			vFeatures.geometryShader = VK_TRUE;
			vFeatures.tessellationShader = VK_TRUE;

			// Device create info.
			VkDeviceCreateInfo vDeviceCreateInfo = {};
			vDeviceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			vDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(vQueueCreateInfos.size());
			vDeviceCreateInfo.pQueueCreateInfos = vQueueCreateInfos.data();
			vDeviceCreateInfo.pEnabledFeatures = &vFeatures;
			vDeviceCreateInfo.enabledExtensionCount = static_cast<uint32>(mDeviceExtensions.size());
			vDeviceCreateInfo.ppEnabledExtensionNames = mDeviceExtensions.data();

			const std::vector<const char*> validationLayers = vInstance.GetValidationLayers();
			if (vInstance.IsValidationEnabled())
			{
				vDeviceCreateInfo.enabledLayerCount = static_cast<uint32>(validationLayers.size());
				vDeviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
			}
			else
				vDeviceCreateInfo.enabledLayerCount = 0;

			// Create the logical device.
			FLINT_VK_ASSERT(vkCreateDevice(vPhysicalDevice, &vDeviceCreateInfo, nullptr, &vLogicalDevice));
			volkLoadDeviceTable(&mDeviceTable, vLogicalDevice);

			if ((mFlags & DeviceFlags::GraphicsCompatible) == DeviceFlags::GraphicsCompatible)
				GetDeviceTable().vkGetDeviceQueue(GetLogicalDevice(), vQueue.mGraphicsFamily.value(), 0, &vQueue.vGraphicsQueue);

			if ((mFlags & DeviceFlags::ComputeCompatible) == DeviceFlags::ComputeCompatible)
				GetDeviceTable().vkGetDeviceQueue(GetLogicalDevice(), vQueue.mComputeFamily.value(), 0, &vQueue.vComputeQueue);

			GetDeviceTable().vkGetDeviceQueue(GetLogicalDevice(), vQueue.mTransferFamily.value(), 0, &vQueue.vTransferQueue);

			// Create the VMA allocator.
			CreateVmaAllocator();
		}

		void VulkanDevice::TerminateLogicalDevice()
		{
			GetDeviceTable().vkDestroyDevice(vLogicalDevice, nullptr);
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

			FLINT_VK_ASSERT(vmaCreateAllocator(&vmaCreateInfo, &mVmaAllocator));
		}

		void VulkanDevice::DestroyVmaAllocator() const
		{
			vmaDestroyAllocator(mVmaAllocator);
		}
	}
}