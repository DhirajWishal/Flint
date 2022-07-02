// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanDevice.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanCommandBuffers.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanRasterizer.hpp"
#include "VulkanBackend/VulkanRayTracer.hpp"
#include "VulkanBackend/VulkanWindow.hpp"
#include "VulkanBackend/VulkanRasterizingProgram.hpp"
#include "VulkanBackend/VulkanStaticModel.hpp"
#include "VUlkanBackend/VulkanTexture2D.hpp"
#include "VUlkanBackend/VulkanTextureSampler.hpp"

#define XXH_INLINE_ALL
#include <xxhash.h>

#include <set>
#include <array>

#ifdef FLINT_PLATFORM_WINDOWS
#	include <execution>

#endif

namespace /* anonymous */
{
	/**
	 * Check if the physical device supports the required queues.
	 *
	 * @param physicalDevice The physical device to check.
	 * @param flag The queue flag to check.
	 * @return Whether or not the queues are supported.
	 */
	bool CheckQueueSupport(VkPhysicalDevice physicalDevice, VkQueueFlagBits flag)
	{
		// Get the queue family count.
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		// Validate if we have queue families.
		if (queueFamilyCount == 0)
			throw Flint::BackendError("Failed to get the queue family property count!");

		// Get the queue family properties.
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		// Iterate over those queue family properties and check if we have a family with the required flag.
		for (const auto& family : queueFamilies)
		{
			if (family.queueCount == 0)
				continue;

			// Check if the queue flag contains what we want.
			if (family.queueFlags & flag)
				return true;
		}

		return false;
	}

	/**
	 * Get the queue family of a queue.
	 *
	 * @param physicalDevice The physical device to get the queue family from.
	 * @param flag The queue flag.
	 * @retrun The queue family.
	 */
	uint32_t GetQueueFamily(VkPhysicalDevice physicalDevice, VkQueueFlagBits flag)
	{
		// Get the queue family count.
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		// Validate if we have queue families.
		if (queueFamilyCount == 0)
			throw Flint::BackendError("Failed to get the queue family property count!");

		// Get the queue family properties.
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		// Iterate over those queue family properties and check if we have a family with the required flag.
		for (uint32_t i = 0; i < queueFamilies.size(); ++i)
		{
			const auto& family = queueFamilies[i];
			if (family.queueCount == 0)
				continue;

			// Check if the queue flag contains what we want.
			if (family.queueFlags & flag)
				return i;
		}

		return -1;
	}

	/**
	 * Check device extension support.
	 *
	 * @param physicalDevice The physical device to check.
	 * @param deviceExtensions The extension to check.
	 */
	bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice, const std::vector<const char*>& deviceExtensions)
	{
		// If there are no extension to check, we can just return true.
		if (deviceExtensions.empty())
			return true;

		// Get the extension count.
		uint32_t extensionCount = 0;
		FLINT_VK_ASSERT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr), "Failed to enumerate physical device extension property count!");

		// Load the extensions.
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		FLINT_VK_ASSERT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data()), "Failed to enumerate physical device extension properties!");

		std::set<std::string_view> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		// Iterate and check if it contains the extensions we need. If it does, remove them from the set so we can later check if 
		// all the required extensions exist.
		for (const VkExtensionProperties& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		// If the required extensions set is empty, it means that all the required extensions exist within the physical device.
		return requiredExtensions.empty();
	}
}

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanDevice::VulkanDevice(const std::shared_ptr<VulkanInstance>& pInstance)
			: Device(pInstance)
		{
			// Select a physical device.
			selectPhysicalDevice();

			// Create the logical device.
			createLogicalDevice();

			// Create the VMA allocator.
			createVMAAllocator();

			// Make sure to set the object as valid.
			validate();
		}

		VulkanDevice::~VulkanDevice()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		std::shared_ptr<Flint::Buffer> VulkanDevice::createBuffer(uint64_t size, BufferUsage usage, const std::byte* pDataStore /*= nullptr*/)
		{
			return std::make_shared<VulkanBuffer>(shared_from_this(), size, usage, pDataStore);
		}

		std::shared_ptr<Flint::Rasterizer> VulkanDevice::createRasterizer(Camera& camera, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions, Multisample multisample /*= Multisample::One*/, bool exclusiveBuffering /*= false*/)
		{
			return std::make_shared<VulkanRasterizer>(shared_from_this(), camera, frameCount, std::move(attachmentDescriptions), multisample, exclusiveBuffering);
		}

		std::shared_ptr<Flint::RayTracer> VulkanDevice::createRayTracer(Camera& camera, uint32_t frameCount)
		{
			return std::make_shared<VulkanRayTracer>(shared_from_this(), camera, frameCount);
		}

		std::shared_ptr<Flint::Window> VulkanDevice::createWindow(std::string&& title, uint32_t width /*= -1*/, uint32_t height /*= -1*/)
		{
			return std::make_shared<VulkanWindow>(shared_from_this(), std::move(title), width, height);
		}

		std::shared_ptr<Flint::RasterizingProgram> VulkanDevice::createRasterizingProgram(ShaderCode&& vertexShader, ShaderCode&& fragementShader)
		{
			return std::make_shared<VulkanRasterizingProgram>(shared_from_this(), std::move(vertexShader), std::move(fragementShader));
		}

		std::shared_ptr<Flint::StaticModel> VulkanDevice::createStaticModel(std::filesystem::path&& assetFile)
		{
			return std::make_shared<VulkanStaticModel>(shared_from_this(), std::move(assetFile));
		}

		std::shared_ptr<Flint::Texture2D> VulkanDevice::createTexture2D(uint32_t width, uint32_t height, ImageUsage usage, PixelFormat format, uint32_t mipLevels /*= 0*/, Multisample multisampleCount /*= Multisample::One*/, const std::byte* pDataStore /*= nullptr*/)
		{
			return std::make_shared<VulkanTexture2D>(shared_from_this(), width, height, usage, format, mipLevels, multisampleCount, pDataStore);
		}

		std::shared_ptr<Flint::TextureSampler> VulkanDevice::createTextureSampler(TextureSamplerSpecification&& specification)
		{
			const auto hash = static_cast<uint64_t>(XXH64(&specification, sizeof(TextureSamplerSpecification), 0));
			if (!m_Samplers.contains(hash))
				m_Samplers[hash] = std::make_shared<VulkanTextureSampler>(shared_from_this(), std::move(specification));

			return m_Samplers[hash];
		}

		void VulkanDevice::terminate()
		{
			// Wait idle to make sure that we don't have anything running at the moment.
			waitIdle();

			// Terminate the samplers.
			m_Samplers.clear();

			// Destroy the VMA allocator.
			destroyVMAAllocator();

			// Destroy the logical device.
			destroyLogicalDevice();

			invalidate();
		}

		void VulkanDevice::waitIdle()
		{
			FLINT_VK_ASSERT(getDeviceTable().vkDeviceWaitIdle(m_LogicalDevice), "Failed to wait idle!");
		}

		Flint::PixelFormat VulkanDevice::getBestDepthFormat() const
		{
			return Utility::GetPixelFormat(Utility::FindDepthFormat(this));
		}

		Flint::Multisample VulkanDevice::getMaximumMultisample() const
		{
			const VkSampleCountFlags counts = m_PhysicalDeviceProperties.limits.framebufferColorSampleCounts & m_PhysicalDeviceProperties.limits.framebufferDepthSampleCounts;

			if (counts & VK_SAMPLE_COUNT_64_BIT)	return Multisample::SixtyFour;
			if (counts & VK_SAMPLE_COUNT_32_BIT)	return Multisample::ThirtyTwo;
			if (counts & VK_SAMPLE_COUNT_16_BIT)	return Multisample::Sixteen;
			if (counts & VK_SAMPLE_COUNT_8_BIT)		return Multisample::Eight;
			if (counts & VK_SAMPLE_COUNT_4_BIT)		return Multisample::Four;
			if (counts & VK_SAMPLE_COUNT_2_BIT)		return Multisample::Two;

			return Multisample::One;
		}

		void VulkanDevice::selectPhysicalDevice()
		{
			// Set up the device extensions.
			m_DeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			m_DeviceExtensions.emplace_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);

			// Enumerate physical devices.
			uint32_t deviceCount = 0;
			FLINT_VK_ASSERT(vkEnumeratePhysicalDevices(getInstance().as<VulkanInstance>()->getInstance(), &deviceCount, nullptr), "Failed to enumerate physical devices.");

			// Throw an error if there are no physical devices available.
			if (deviceCount == 0)
				throw BackendError("No physical devices found!");

			std::vector<VkPhysicalDevice> candidates(deviceCount);
			FLINT_VK_ASSERT(vkEnumeratePhysicalDevices(getInstance().as<VulkanInstance>()->getInstance(), &deviceCount, candidates.data()), "Failed to enumerate physical devices.");

			struct Candidate { VkPhysicalDeviceProperties m_Properties; VkPhysicalDevice m_Candidate; };
			std::array<Candidate, 6> priorityMap = { Candidate() };

			// Iterate through all the candidate devices and find the best device.
			for (const auto& candidate : candidates)
			{
				// Check if the device is suitable for our use.
				if (CheckDeviceExtensionSupport(candidate, m_DeviceExtensions) &&
					CheckQueueSupport(candidate, VK_QUEUE_GRAPHICS_BIT) &&
					CheckQueueSupport(candidate, VK_QUEUE_COMPUTE_BIT) &&
					CheckQueueSupport(candidate, VK_QUEUE_TRANSFER_BIT))
				{
					VkPhysicalDeviceProperties physicalDeviceProperties = {};
					vkGetPhysicalDeviceProperties(candidate, &physicalDeviceProperties);

					// Sort the candidates by priority.
					uint8_t priorityIndex = 5;
					switch (physicalDeviceProperties.deviceType)
					{
					case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
						priorityIndex = 0;
						break;

					case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
						priorityIndex = 1;
						break;

					case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
						priorityIndex = 2;
						break;

					case VK_PHYSICAL_DEVICE_TYPE_CPU:
						priorityIndex = 3;
						break;

					case VK_PHYSICAL_DEVICE_TYPE_OTHER:
						priorityIndex = 4;
						break;

					default:
						priorityIndex = 5;
						break;
					}

					priorityMap[priorityIndex].m_Candidate = candidate;
					priorityMap[priorityIndex].m_Properties = physicalDeviceProperties;
				}
			}

			// Choose the physical device with the highest priority.
			for (const auto& candidate : priorityMap)
			{
				if (candidate.m_Candidate != VK_NULL_HANDLE)
				{
					m_PhysicalDevice = candidate.m_Candidate;
					m_PhysicalDeviceProperties = candidate.m_Properties;
					break;
				}
			}

			// Throw and error if a physical device was not found.
			if (m_PhysicalDevice == VK_NULL_HANDLE)
				throw BackendError("Failed to find a suitable physical device!");

			// Setup the queue families.
			m_GraphicsQueue.m_Family = GetQueueFamily(m_PhysicalDevice, VK_QUEUE_GRAPHICS_BIT);
			m_ComputeQueue.m_Family = GetQueueFamily(m_PhysicalDevice, VK_QUEUE_COMPUTE_BIT);
			m_TransferQueue.m_Family = GetQueueFamily(m_PhysicalDevice, VK_QUEUE_TRANSFER_BIT);
		}

		void VulkanDevice::createLogicalDevice()
		{
			// Setup device queues.
			constexpr float priority = 1.0f;
			std::set<uint32_t> uniqueQueueFamilies = {
				m_GraphicsQueue.m_Family,
				m_ComputeQueue.m_Family,
				m_TransferQueue.m_Family
			};

			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.pNext = nullptr;
			queueCreateInfo.flags = 0;
			queueCreateInfo.queueFamilyIndex = 0;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &priority;

			std::vector< VkDeviceQueueCreateInfo> queueCreateInfos;
			for (const auto& family : uniqueQueueFamilies)
			{
				queueCreateInfo.queueFamilyIndex = family;
				queueCreateInfos.emplace_back(queueCreateInfo);
			}

			// Setup all the required features.
			VkPhysicalDeviceFeatures features = {};
			features.samplerAnisotropy = VK_TRUE;
			features.sampleRateShading = VK_TRUE;
			features.tessellationShader = VK_TRUE;
			features.geometryShader = VK_TRUE;

			// Setup the device create info.
			VkDeviceCreateInfo deviceCreateInfo = {};
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCreateInfo.pNext = nullptr;
			deviceCreateInfo.flags = 0;
			deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
			deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
			deviceCreateInfo.enabledLayerCount = 0;
			deviceCreateInfo.ppEnabledLayerNames = nullptr;
			deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
			deviceCreateInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
			deviceCreateInfo.pEnabledFeatures = &features;

			// Enable validation if requested.
			if (getInstance().as<VulkanInstance>()->isValidationEnabled())
			{
				// Get the validation layers and initialize it.
				deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(getInstance().as<VulkanInstance>()->getValidationLayers().size());
				deviceCreateInfo.ppEnabledLayerNames = getInstance().as<VulkanInstance>()->getValidationLayers().data();
			}

			// Create the device.
			FLINT_VK_ASSERT(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_LogicalDevice), "Failed to create the logical device!");

			// Load the device table.
			volkLoadDeviceTable(&m_DeviceTable, m_LogicalDevice);

			// Get the queues.
			vkGetDeviceQueue(m_LogicalDevice, m_GraphicsQueue.m_Family, 0, &m_GraphicsQueue.m_Queue);
			vkGetDeviceQueue(m_LogicalDevice, m_ComputeQueue.m_Family, 0, &m_ComputeQueue.m_Queue);
			vkGetDeviceQueue(m_LogicalDevice, m_TransferQueue.m_Family, 0, &m_TransferQueue.m_Queue);
		}

		void VulkanDevice::destroyLogicalDevice()
		{
			vkDestroyDevice(m_LogicalDevice, nullptr);
		}

		void VulkanDevice::createVMAAllocator()
		{
			// Setup the Vulkan functions needed by VMA.
			VmaVulkanFunctions functions = {};
			functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
			functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
			functions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
			functions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
			functions.vkAllocateMemory = m_DeviceTable.vkAllocateMemory;
			functions.vkFreeMemory = m_DeviceTable.vkFreeMemory;
			functions.vkMapMemory = m_DeviceTable.vkMapMemory;
			functions.vkUnmapMemory = m_DeviceTable.vkUnmapMemory;
			functions.vkFlushMappedMemoryRanges = m_DeviceTable.vkFlushMappedMemoryRanges;
			functions.vkInvalidateMappedMemoryRanges = m_DeviceTable.vkInvalidateMappedMemoryRanges;
			functions.vkBindBufferMemory = m_DeviceTable.vkBindBufferMemory;
			functions.vkBindImageMemory = m_DeviceTable.vkBindImageMemory;
			functions.vkGetBufferMemoryRequirements = m_DeviceTable.vkGetBufferMemoryRequirements;
			functions.vkGetImageMemoryRequirements = m_DeviceTable.vkGetImageMemoryRequirements;
			functions.vkCreateBuffer = m_DeviceTable.vkCreateBuffer;
			functions.vkDestroyBuffer = m_DeviceTable.vkDestroyBuffer;
			functions.vkCreateImage = m_DeviceTable.vkCreateImage;
			functions.vkDestroyImage = m_DeviceTable.vkDestroyImage;
			functions.vkCmdCopyBuffer = m_DeviceTable.vkCmdCopyBuffer;
			functions.vkGetBufferMemoryRequirements2KHR = m_DeviceTable.vkGetBufferMemoryRequirements2KHR;
			functions.vkGetImageMemoryRequirements2KHR = m_DeviceTable.vkGetImageMemoryRequirements2KHR;
			functions.vkBindBufferMemory2KHR = m_DeviceTable.vkBindBufferMemory2KHR;
			functions.vkBindImageMemory2KHR = m_DeviceTable.vkBindImageMemory2KHR;
			functions.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;
			functions.vkGetDeviceBufferMemoryRequirements = m_DeviceTable.vkGetDeviceBufferMemoryRequirements;
			functions.vkGetDeviceImageMemoryRequirements = m_DeviceTable.vkGetDeviceImageMemoryRequirements;

			// Setup create info.
			VmaAllocatorCreateInfo createInfo = {};
			createInfo.flags = 0;
			createInfo.physicalDevice = m_PhysicalDevice;
			createInfo.device = m_LogicalDevice;
			createInfo.pVulkanFunctions = &functions;
			createInfo.instance = getInstance().as<VulkanInstance>()->getInstance();
			createInfo.vulkanApiVersion = VulkanVersion;

			// Create the allocator.
			FLINT_VK_ASSERT(vmaCreateAllocator(&createInfo, &m_Allocator), "Failed to create the allocator!");
		}

		void VulkanDevice::destroyVMAAllocator()
		{
			vmaDestroyAllocator(m_Allocator);
		}

		namespace Utility
		{
			VkSampleCountFlagBits GetSampleCountFlagBits(Multisample multisample)
			{
				switch (multisample)
				{
				case Flint::Multisample::One:									return VK_SAMPLE_COUNT_1_BIT;
				case Flint::Multisample::Two:									return VK_SAMPLE_COUNT_2_BIT;
				case Flint::Multisample::Four:									return VK_SAMPLE_COUNT_4_BIT;
				case Flint::Multisample::Eight:									return VK_SAMPLE_COUNT_8_BIT;
				case Flint::Multisample::Sixteen:								return VK_SAMPLE_COUNT_16_BIT;
				case Flint::Multisample::ThirtyTwo:								return VK_SAMPLE_COUNT_32_BIT;
				case Flint::Multisample::SixtyFour:								return VK_SAMPLE_COUNT_64_BIT;
				default:														return VK_SAMPLE_COUNT_1_BIT;
				}
			}

			VkFormat GetImageFormat(PixelFormat format)
			{
				switch (format)
				{
				case PixelFormat::Undefined:									return VK_FORMAT_UNDEFINED;
				case PixelFormat::R8_SRGB:										return VK_FORMAT_R8_SRGB;
				case PixelFormat::R8G8_SRGB:									return VK_FORMAT_R8G8_SRGB;
				case PixelFormat::R8G8B8_SRGB:									return VK_FORMAT_R8G8B8_SRGB;
				case PixelFormat::R8G8B8A8_SRGB:								return VK_FORMAT_R8G8B8A8_SRGB;
				case PixelFormat::R8_UNORMAL:									return VK_FORMAT_R8_UNORM;
				case PixelFormat::R8G8_UNORMAL:									return VK_FORMAT_R8G8_UNORM;
				case PixelFormat::R8G8B8_UNORMAL:								return VK_FORMAT_R8G8B8_UNORM;
				case PixelFormat::R8G8B8A8_UNORMAL:								return VK_FORMAT_R8G8B8A8_UNORM;
				case PixelFormat::B8G8R8_SRGB:									return VK_FORMAT_B8G8R8_SRGB;
				case PixelFormat::B8G8R8A8_SRGB:								return VK_FORMAT_B8G8R8A8_SRGB;
				case PixelFormat::B8G8R8_UNORMAL:								return VK_FORMAT_B8G8R8_UNORM;
				case PixelFormat::B8G8R8A8_UNORMAL:								return VK_FORMAT_B8G8R8A8_UNORM;
				case PixelFormat::R16_SFLOAT:									return VK_FORMAT_R16_SFLOAT;
				case PixelFormat::R16G16_SFLOAT:								return VK_FORMAT_R16G16_SFLOAT;
				case PixelFormat::R16G16B16_SFLOAT:								return VK_FORMAT_R16G16B16_SFLOAT;
				case PixelFormat::R16G16B16A16_SFLOAT:							return VK_FORMAT_R16G16B16A16_SFLOAT;
				case PixelFormat::R32_SFLOAT:									return VK_FORMAT_R32_SFLOAT;
				case PixelFormat::R32G32_SFLOAT:								return VK_FORMAT_R32G32_SFLOAT;
				case PixelFormat::R32G32B32_SFLOAT:								return VK_FORMAT_R32G32B32_SFLOAT;
				case PixelFormat::R32G32B32A32_SFLOAT:							return VK_FORMAT_R32G32B32A32_SFLOAT;
				case PixelFormat::D16_SINT:										return VK_FORMAT_D16_UNORM;
				case PixelFormat::D32_SFLOAT:									return VK_FORMAT_D32_SFLOAT;
				case PixelFormat::S8_UINT:										return VK_FORMAT_S8_UINT;
				case PixelFormat::D16_UNORMAL_S8_UINT:							return VK_FORMAT_D16_UNORM_S8_UINT;
				case PixelFormat::D24_UNORMAL_S8_UINT:							return VK_FORMAT_D24_UNORM_S8_UINT;
				case PixelFormat::D32_SFLOAT_S8_UINT:							return VK_FORMAT_D32_SFLOAT_S8_UINT;
				default:														throw BackendError("Invalid pixel format!");
				}

				return VK_FORMAT_UNDEFINED;
			}

			PixelFormat GetPixelFormat(VkFormat format)
			{
				switch (format)
				{
				case VK_FORMAT_UNDEFINED:										return PixelFormat::Undefined;
				case VK_FORMAT_R8_SRGB:											return PixelFormat::R8_SRGB;
				case VK_FORMAT_R8G8_SRGB:										return PixelFormat::R8G8_SRGB;
				case VK_FORMAT_R8G8B8_SRGB:										return PixelFormat::R8G8B8_SRGB;
				case VK_FORMAT_R8G8B8A8_SRGB:									return PixelFormat::R8G8B8A8_SRGB;
				case VK_FORMAT_R8_UNORM:										return PixelFormat::R8_UNORMAL;
				case VK_FORMAT_R8G8_UNORM:										return PixelFormat::R8G8_UNORMAL;
				case VK_FORMAT_R8G8B8_UNORM:									return PixelFormat::R8G8B8_UNORMAL;
				case VK_FORMAT_R8G8B8A8_UNORM:									return PixelFormat::R8G8B8A8_UNORMAL;
				case VK_FORMAT_B8G8R8_SRGB:										return PixelFormat::B8G8R8_SRGB;
				case VK_FORMAT_B8G8R8A8_SRGB:									return PixelFormat::B8G8R8A8_SRGB;
				case VK_FORMAT_B8G8R8_UNORM:									return PixelFormat::B8G8R8_UNORMAL;
				case VK_FORMAT_B8G8R8A8_UNORM:									return PixelFormat::B8G8R8A8_UNORMAL;
				case VK_FORMAT_R16_SFLOAT:										return PixelFormat::R16_SFLOAT;
				case VK_FORMAT_R16G16_SFLOAT:									return PixelFormat::R16G16_SFLOAT;
				case VK_FORMAT_R16G16B16_SFLOAT:								return PixelFormat::R16G16B16_SFLOAT;
				case VK_FORMAT_R16G16B16A16_SFLOAT:								return PixelFormat::R16G16B16A16_SFLOAT;
				case VK_FORMAT_R32_SFLOAT:										return PixelFormat::R32_SFLOAT;
				case VK_FORMAT_R32G32_SFLOAT:									return PixelFormat::R32G32_SFLOAT;
				case VK_FORMAT_R32G32B32_SFLOAT:								return PixelFormat::R32G32B32_SFLOAT;
				case VK_FORMAT_R32G32B32A32_SFLOAT:								return PixelFormat::R32G32B32A32_SFLOAT;
				case VK_FORMAT_D16_UNORM:										return PixelFormat::D16_SINT;
				case VK_FORMAT_D32_SFLOAT:										return PixelFormat::D32_SFLOAT;
				default:														throw BackendError("Unsupported format!");
				}

				return PixelFormat::Undefined;
			}

			VkPipelineStageFlags GetPipelineStageFlags(VkAccessFlags flags)
			{
				switch (flags)
				{
				case VK_ACCESS_INDIRECT_COMMAND_READ_BIT:						return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
				case VK_ACCESS_INDEX_READ_BIT:									return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
				case VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT:						return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
				case VK_ACCESS_UNIFORM_READ_BIT:								return VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
				case VK_ACCESS_SHADER_READ_BIT:									return VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
				case VK_ACCESS_SHADER_WRITE_BIT:								return VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
				case VK_ACCESS_INPUT_ATTACHMENT_READ_BIT:						return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				case VK_ACCESS_COLOR_ATTACHMENT_READ_BIT:						return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				case VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT:						return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				case VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT:				return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				case VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT:				return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				case VK_ACCESS_TRANSFER_READ_BIT:								return VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
				case VK_ACCESS_TRANSFER_WRITE_BIT:								return VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
				case VK_ACCESS_HOST_READ_BIT:									return VK_PIPELINE_STAGE_HOST_BIT;
				case VK_ACCESS_HOST_WRITE_BIT:									return VK_PIPELINE_STAGE_HOST_BIT;
				case VK_ACCESS_MEMORY_READ_BIT:									return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
				case VK_ACCESS_MEMORY_WRITE_BIT:								return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
				case VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT:		return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				case VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV:					return VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV;
				case VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV:					return VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV;
				case VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT:				return VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT;
				case VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR:	return VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
				case VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT:				return VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;
				case VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT:		return VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;
				case VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT:			return VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT | VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
				case VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR:				return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
				case VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR:			return VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
				case VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT:				return VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT;
				default:														return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
				}
			}

			VkFormat FindSupportedFormat(const VulkanDevice* pDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
			{
				for (VkFormat format : candidates)
				{
					VkFormatProperties props = {};
					vkGetPhysicalDeviceFormatProperties(pDevice->getPhysicalDevice(), format, &props);

					if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
						return format;

					else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
						return format;
				}

				throw BackendError("Unable to find suitable format!");
			}

			bool HasStencilComponent(VkFormat format)
			{
				return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
			}

			VkFormat FindDepthFormat(const VulkanDevice* pDevice)
			{
				return FindSupportedFormat(
					pDevice,
					{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
					VK_IMAGE_TILING_OPTIMAL,
					VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
				);
			}

			VkDescriptorType GetDescriptorType(Flint::ResourceType type)
			{
				switch (type)
				{
				case Flint::ResourceType::Sampler:								return VK_DESCRIPTOR_TYPE_SAMPLER;
				case Flint::ResourceType::CombinedImageSampler:					return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				case Flint::ResourceType::SampledImage:							return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				case Flint::ResourceType::StorageImage:							return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				case Flint::ResourceType::UniformTexelBuffer:					return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
				case Flint::ResourceType::StorageTexelBuffer:					return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
				case Flint::ResourceType::UniformBuffer:						return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				case Flint::ResourceType::StorageBuffer:						return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				case Flint::ResourceType::DynamicUniformBuffer:					return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
				case Flint::ResourceType::DynamicStorageBuffer:					return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
				case Flint::ResourceType::InputAttachment:						return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
				case Flint::ResourceType::AccelerationStructure:				return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
				default:														throw Flint::BackendError("Invalid or Undefined shader resource type!");
				}
			}

			uint8_t GetSizeFromFormat(VkFormat format)
			{
				switch (format)
				{
				case VkFormat::VK_FORMAT_R8_UNORM:
				case VkFormat::VK_FORMAT_R8_SNORM:
				case VkFormat::VK_FORMAT_R8_USCALED:
				case VkFormat::VK_FORMAT_R8_SSCALED:
				case VkFormat::VK_FORMAT_R8_UINT:
				case VkFormat::VK_FORMAT_R8_SINT:
				case VkFormat::VK_FORMAT_R8_SRGB:
				case VkFormat::VK_FORMAT_S8_UINT:
					return 1;

				case VkFormat::VK_FORMAT_R8G8_UNORM:
				case VkFormat::VK_FORMAT_R8G8_SNORM:
				case VkFormat::VK_FORMAT_R8G8_USCALED:
				case VkFormat::VK_FORMAT_R8G8_SSCALED:
				case VkFormat::VK_FORMAT_R8G8_UINT:
				case VkFormat::VK_FORMAT_R8G8_SINT:
				case VkFormat::VK_FORMAT_R8G8_SRGB:
				case VkFormat::VK_FORMAT_R16_UNORM:
				case VkFormat::VK_FORMAT_R16_SNORM:
				case VkFormat::VK_FORMAT_R16_USCALED:
				case VkFormat::VK_FORMAT_R16_SSCALED:
				case VkFormat::VK_FORMAT_R16_UINT:
				case VkFormat::VK_FORMAT_R16_SINT:
				case VkFormat::VK_FORMAT_R16_SFLOAT:
				case VkFormat::VK_FORMAT_D16_UNORM:
					return 2;

				case VkFormat::VK_FORMAT_R8G8B8_UNORM:
				case VkFormat::VK_FORMAT_R8G8B8_SNORM:
				case VkFormat::VK_FORMAT_R8G8B8_USCALED:
				case VkFormat::VK_FORMAT_R8G8B8_SSCALED:
				case VkFormat::VK_FORMAT_R8G8B8_UINT:
				case VkFormat::VK_FORMAT_R8G8B8_SINT:
				case VkFormat::VK_FORMAT_R8G8B8_SRGB:
				case VkFormat::VK_FORMAT_B8G8R8_UNORM:
				case VkFormat::VK_FORMAT_B8G8R8_SNORM:
				case VkFormat::VK_FORMAT_B8G8R8_USCALED:
				case VkFormat::VK_FORMAT_B8G8R8_SSCALED:
				case VkFormat::VK_FORMAT_B8G8R8_UINT:
				case VkFormat::VK_FORMAT_B8G8R8_SINT:
				case VkFormat::VK_FORMAT_B8G8R8_SRGB:
				case VkFormat::VK_FORMAT_D16_UNORM_S8_UINT:
					return 3;

				case VkFormat::VK_FORMAT_R8G8B8A8_UNORM:
				case VkFormat::VK_FORMAT_R8G8B8A8_SNORM:
				case VkFormat::VK_FORMAT_R8G8B8A8_USCALED:
				case VkFormat::VK_FORMAT_R8G8B8A8_SSCALED:
				case VkFormat::VK_FORMAT_R8G8B8A8_UINT:
				case VkFormat::VK_FORMAT_R8G8B8A8_SINT:
				case VkFormat::VK_FORMAT_R8G8B8A8_SRGB:
				case VkFormat::VK_FORMAT_B8G8R8A8_UNORM:
				case VkFormat::VK_FORMAT_B8G8R8A8_SNORM:
				case VkFormat::VK_FORMAT_B8G8R8A8_USCALED:
				case VkFormat::VK_FORMAT_B8G8R8A8_SSCALED:
				case VkFormat::VK_FORMAT_B8G8R8A8_UINT:
				case VkFormat::VK_FORMAT_B8G8R8A8_SINT:
				case VkFormat::VK_FORMAT_B8G8R8A8_SRGB:
				case VkFormat::VK_FORMAT_A8B8G8R8_UNORM_PACK32:
				case VkFormat::VK_FORMAT_A8B8G8R8_SNORM_PACK32:
				case VkFormat::VK_FORMAT_A8B8G8R8_USCALED_PACK32:
				case VkFormat::VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
				case VkFormat::VK_FORMAT_A8B8G8R8_UINT_PACK32:
				case VkFormat::VK_FORMAT_A8B8G8R8_SINT_PACK32:
				case VkFormat::VK_FORMAT_A8B8G8R8_SRGB_PACK32:
				case VkFormat::VK_FORMAT_A2R10G10B10_UNORM_PACK32:
				case VkFormat::VK_FORMAT_A2R10G10B10_SNORM_PACK32:
				case VkFormat::VK_FORMAT_A2R10G10B10_USCALED_PACK32:
				case VkFormat::VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
				case VkFormat::VK_FORMAT_A2R10G10B10_UINT_PACK32:
				case VkFormat::VK_FORMAT_A2R10G10B10_SINT_PACK32:
				case VkFormat::VK_FORMAT_A2B10G10R10_UNORM_PACK32:
				case VkFormat::VK_FORMAT_A2B10G10R10_SNORM_PACK32:
				case VkFormat::VK_FORMAT_A2B10G10R10_USCALED_PACK32:
				case VkFormat::VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
				case VkFormat::VK_FORMAT_A2B10G10R10_UINT_PACK32:
				case VkFormat::VK_FORMAT_A2B10G10R10_SINT_PACK32:
				case VkFormat::VK_FORMAT_R16G16_UNORM:
				case VkFormat::VK_FORMAT_R16G16_SNORM:
				case VkFormat::VK_FORMAT_R16G16_USCALED:
				case VkFormat::VK_FORMAT_R16G16_SSCALED:
				case VkFormat::VK_FORMAT_R16G16_UINT:
				case VkFormat::VK_FORMAT_R16G16_SINT:
				case VkFormat::VK_FORMAT_R16G16_SFLOAT:
				case VkFormat::VK_FORMAT_R32_UINT:
				case VkFormat::VK_FORMAT_R32_SINT:
				case VkFormat::VK_FORMAT_R32_SFLOAT:
				case VkFormat::VK_FORMAT_B10G11R11_UFLOAT_PACK32:
				case VkFormat::VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
				case VkFormat::VK_FORMAT_X8_D24_UNORM_PACK32:
				case VkFormat::VK_FORMAT_D32_SFLOAT:
				case VkFormat::VK_FORMAT_D24_UNORM_S8_UINT:
					return 4;

				case VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT:
					return 5;

				case VkFormat::VK_FORMAT_R16G16B16_UNORM:
				case VkFormat::VK_FORMAT_R16G16B16_SNORM:
				case VkFormat::VK_FORMAT_R16G16B16_USCALED:
				case VkFormat::VK_FORMAT_R16G16B16_SSCALED:
				case VkFormat::VK_FORMAT_R16G16B16_UINT:
				case VkFormat::VK_FORMAT_R16G16B16_SINT:
				case VkFormat::VK_FORMAT_R16G16B16_SFLOAT:
					return 6;

				case VkFormat::VK_FORMAT_R16G16B16A16_UNORM:
				case VkFormat::VK_FORMAT_R16G16B16A16_SNORM:
				case VkFormat::VK_FORMAT_R16G16B16A16_USCALED:
				case VkFormat::VK_FORMAT_R16G16B16A16_SSCALED:
				case VkFormat::VK_FORMAT_R16G16B16A16_UINT:
				case VkFormat::VK_FORMAT_R16G16B16A16_SINT:
				case VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT:
				case VkFormat::VK_FORMAT_R32G32_UINT:
				case VkFormat::VK_FORMAT_R32G32_SINT:
				case VkFormat::VK_FORMAT_R32G32_SFLOAT:
				case VkFormat::VK_FORMAT_R64_UINT:
				case VkFormat::VK_FORMAT_R64_SINT:
				case VkFormat::VK_FORMAT_R64_SFLOAT:
					return 8;

				case VkFormat::VK_FORMAT_R32G32B32_UINT:
				case VkFormat::VK_FORMAT_R32G32B32_SINT:
				case VkFormat::VK_FORMAT_R32G32B32_SFLOAT:
					return 12;

				case VkFormat::VK_FORMAT_R32G32B32A32_UINT:
				case VkFormat::VK_FORMAT_R32G32B32A32_SINT:
				case VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT:
				case VkFormat::VK_FORMAT_R64G64_UINT:
				case VkFormat::VK_FORMAT_R64G64_SINT:
				case VkFormat::VK_FORMAT_R64G64_SFLOAT:
					return 16;

				case VkFormat::VK_FORMAT_R64G64B64_UINT:
				case VkFormat::VK_FORMAT_R64G64B64_SINT:
				case VkFormat::VK_FORMAT_R64G64B64_SFLOAT:
					return 24;

				case VkFormat::VK_FORMAT_R64G64B64A64_UINT:
				case VkFormat::VK_FORMAT_R64G64B64A64_SINT:
				case VkFormat::VK_FORMAT_R64G64B64A64_SFLOAT:
					return 32;

				case VkFormat::VK_FORMAT_BC1_RGB_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_BC1_RGB_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_BC2_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_BC2_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_BC3_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_BC3_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_BC4_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_BC4_SNORM_BLOCK:
				case VkFormat::VK_FORMAT_BC5_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_BC5_SNORM_BLOCK:
				case VkFormat::VK_FORMAT_BC6H_UFLOAT_BLOCK:
				case VkFormat::VK_FORMAT_BC6H_SFLOAT_BLOCK:
				case VkFormat::VK_FORMAT_BC7_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_BC7_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_EAC_R11_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_EAC_R11_SNORM_BLOCK:
				case VkFormat::VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
				case VkFormat::VK_FORMAT_G8B8G8R8_422_UNORM:
				case VkFormat::VK_FORMAT_B8G8R8G8_422_UNORM:
				case VkFormat::VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
				case VkFormat::VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
				case VkFormat::VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
				case VkFormat::VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
				case VkFormat::VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
				case VkFormat::VK_FORMAT_R10X6_UNORM_PACK16:
				case VkFormat::VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
				case VkFormat::VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
				case VkFormat::VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
				case VkFormat::VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
				case VkFormat::VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
				case VkFormat::VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
				case VkFormat::VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
				case VkFormat::VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
				case VkFormat::VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
				case VkFormat::VK_FORMAT_R12X4_UNORM_PACK16:
				case VkFormat::VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
				case VkFormat::VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
				case VkFormat::VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
				case VkFormat::VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
				case VkFormat::VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
				case VkFormat::VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
				case VkFormat::VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
				case VkFormat::VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
				case VkFormat::VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
				case VkFormat::VK_FORMAT_G16B16G16R16_422_UNORM:
				case VkFormat::VK_FORMAT_B16G16R16G16_422_UNORM:
				case VkFormat::VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
				case VkFormat::VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
				case VkFormat::VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
				case VkFormat::VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
				case VkFormat::VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
				case VkFormat::VK_FORMAT_G8_B8R8_2PLANE_444_UNORM:
				case VkFormat::VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16:
				case VkFormat::VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16:
				case VkFormat::VK_FORMAT_G16_B16R16_2PLANE_444_UNORM:
				case VkFormat::VK_FORMAT_A4R4G4B4_UNORM_PACK16:
				case VkFormat::VK_FORMAT_A4B4G4R4_UNORM_PACK16:
				case VkFormat::VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK:
				case VkFormat::VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK:
				case VkFormat::VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
				case VkFormat::VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
				case VkFormat::VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
				case VkFormat::VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
				case VkFormat::VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
				case VkFormat::VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
				case VkFormat::VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
				case VkFormat::VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
				case VkFormat::VK_FORMAT_MAX_ENUM:
					break;
				}

				return 0;
			}

			VkFormat GetVkFormat(Flint::DataType type)
			{
				switch (type)
				{
				case Flint::DataType::Float:									return VK_FORMAT_R32_SFLOAT;
				case Flint::DataType::Vec2_8:									return VK_FORMAT_R8G8_UINT;
				case Flint::DataType::Vec2_16:									return VK_FORMAT_R16G16_SFLOAT;
				case Flint::DataType::Vec2_32:									return VK_FORMAT_R32G32_SFLOAT;
				case Flint::DataType::Vec2_64:									return VK_FORMAT_R64G64_SFLOAT;
				case Flint::DataType::Vec3_8:									return VK_FORMAT_R8G8B8_UINT;
				case Flint::DataType::Vec3_16:									return VK_FORMAT_R16G16B16_SFLOAT;
				case Flint::DataType::Vec3_32:									return VK_FORMAT_R32G32B32_SFLOAT;
				case Flint::DataType::Vec3_64:									return VK_FORMAT_R64G64B64_SFLOAT;
				case Flint::DataType::Vec4_8:									return VK_FORMAT_R8G8B8A8_UINT;
				case Flint::DataType::Vec4_16:									return VK_FORMAT_R16G16B16A16_SFLOAT;
				case Flint::DataType::Vec4_32:									return VK_FORMAT_R32G32B32A32_SFLOAT;
				case Flint::DataType::Vec4_64:									return VK_FORMAT_R64G64B64A64_SFLOAT;
				default:														throw Flint::BackendError("Invalid vertex attribute type!");
				}
			}
		}
	}
}