// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanEngine.hpp"
#include "VulkanBackend/VulkanMacros.hpp"
#include "VulkanBackend/VulkanWindow.hpp"

#include <set>
#include <array>

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
		VulkanEngine::VulkanEngine(VulkanInstance& instance)
			: Engine(instance)
		{
			// Select a physical device.
			selectPhysicalDevice();

			// Create the logical device.
			createLogicalDevice();

			// Create the VMA allocator.
			createVMAAllocator();
		}

		VulkanEngine::~VulkanEngine()
		{
			// Destroy the VMA allocator.
			destroyVMAAllocator();

			// Destroy the logical device.
			destroyLogicalDevice();
		}

		void VulkanEngine::waitIdle()
		{
			FLINT_VK_ASSERT(getDeviceTable().vkDeviceWaitIdle(m_LogicalDevice), "Failed to wait idle!");
		}

		Flint::Multisample VulkanEngine::getMaximumMultisample() const
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

		std::unique_ptr<Flint::Window> VulkanEngine::createWindow(std::string&& title, uint32_t width /*= -1*/, uint32_t height /*= -1*/)
		{
			return std::make_unique<VulkanWindow>(*this, std::move(title), width, height);
		}

		void VulkanEngine::selectPhysicalDevice()
		{
			// Set up the device extensions.
			m_DeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

			// Enumerate physical devices.
			uint32_t deviceCount = 0;
			FLINT_VK_ASSERT(vkEnumeratePhysicalDevices(getInstanceAs<VulkanInstance>().getInstance(), &deviceCount, nullptr), "Failed to enumerate physical devices.");

			// Throw an error if there are no physical devices available.
			if (deviceCount == 0)
				throw BackendError("No physical devices found!");

			std::vector<VkPhysicalDevice> candidates(deviceCount);
			FLINT_VK_ASSERT(vkEnumeratePhysicalDevices(getInstanceAs<VulkanInstance>().getInstance(), &deviceCount, candidates.data()), "Failed to enumerate physical devices.");

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

		void VulkanEngine::createLogicalDevice()
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

			if (getInstanceAs<VulkanInstance>().isValidationEnabled())
			{
				// Get the validation layers and initialize it.
				deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(getInstanceAs<VulkanInstance>().getValidationLayers().size());
				deviceCreateInfo.ppEnabledLayerNames = getInstanceAs<VulkanInstance>().getValidationLayers().data();
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

		void VulkanEngine::destroyLogicalDevice()
		{
			vkDestroyDevice(m_LogicalDevice, nullptr);
		}

		void VulkanEngine::createVMAAllocator()
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
			createInfo.instance = getInstanceAs<VulkanInstance>().getInstance();
			createInfo.vulkanApiVersion = volkGetInstanceVersion();

			// Create the allocator.
			FLINT_VK_ASSERT(vmaCreateAllocator(&createInfo, &m_Allocator), "Failed to create the allocator!");
		}

		void VulkanEngine::destroyVMAAllocator()
		{
			vmaDestroyAllocator(m_Allocator);
		}

		namespace Utility
		{
			VkSampleCountFlagBits GetSampleCountFlagBits(Multisample multisample)
			{
				switch (multisample)
				{
				case Flint::Multisample::One:				return VK_SAMPLE_COUNT_1_BIT;
				case Flint::Multisample::Two:				return VK_SAMPLE_COUNT_2_BIT;
				case Flint::Multisample::Four:				return VK_SAMPLE_COUNT_4_BIT;
				case Flint::Multisample::Eight:				return VK_SAMPLE_COUNT_8_BIT;
				case Flint::Multisample::Sixteen:			return VK_SAMPLE_COUNT_16_BIT;
				case Flint::Multisample::ThirtyTwo:			return VK_SAMPLE_COUNT_32_BIT;
				case Flint::Multisample::SixtyFour:			return VK_SAMPLE_COUNT_64_BIT;
				default:									return VK_SAMPLE_COUNT_1_BIT;
				}
			}

			VkFormat GetImageFormat(PixelFormat format)
			{
				switch (format)
				{
				case PixelFormat::Undefined:							return VK_FORMAT_UNDEFINED;
				case PixelFormat::R8_SRGB:								return VK_FORMAT_R8_SRGB;
				case PixelFormat::R8G8_SRGB:							return VK_FORMAT_R8G8_SRGB;
				case PixelFormat::R8G8B8_SRGB:							return VK_FORMAT_R8G8B8_SRGB;
				case PixelFormat::R8G8B8A8_SRGB:						return VK_FORMAT_R8G8B8A8_SRGB;
				case PixelFormat::R8_UNORMAL:							return VK_FORMAT_R8_UNORM;
				case PixelFormat::R8G8_UNORMAL:							return VK_FORMAT_R8G8_UNORM;
				case PixelFormat::R8G8B8_UNORMAL:						return VK_FORMAT_R8G8B8_UNORM;
				case PixelFormat::R8G8B8A8_UNORMAL:						return VK_FORMAT_R8G8B8A8_UNORM;
				case PixelFormat::B8G8R8_SRGB:							return VK_FORMAT_B8G8R8_SRGB;
				case PixelFormat::B8G8R8A8_SRGB:						return VK_FORMAT_B8G8R8A8_SRGB;
				case PixelFormat::B8G8R8_UNORMAL:						return VK_FORMAT_B8G8R8_UNORM;
				case PixelFormat::B8G8R8A8_UNORMAL:						return VK_FORMAT_B8G8R8A8_UNORM;
				case PixelFormat::R16_SFLOAT:							return VK_FORMAT_R16_SFLOAT;
				case PixelFormat::R16G16_SFLOAT:						return VK_FORMAT_R16G16_SFLOAT;
				case PixelFormat::R16G16B16_SFLOAT:						return VK_FORMAT_R16G16B16_SFLOAT;
				case PixelFormat::R16G16B16A16_SFLOAT:					return VK_FORMAT_R16G16B16A16_SFLOAT;
				case PixelFormat::R32_SFLOAT:							return VK_FORMAT_R32_SFLOAT;
				case PixelFormat::R32G32_SFLOAT:						return VK_FORMAT_R32G32_SFLOAT;
				case PixelFormat::R32G32B32_SFLOAT:						return VK_FORMAT_R32G32B32_SFLOAT;
				case PixelFormat::R32G32B32A32_SFLOAT:					return VK_FORMAT_R32G32B32A32_SFLOAT;
				case PixelFormat::D16_SINT:								return VK_FORMAT_D16_UNORM;
				case PixelFormat::D32_SFLOAT:							return VK_FORMAT_D32_SFLOAT;
				case PixelFormat::S8_UINT:								return VK_FORMAT_S8_UINT;
				case PixelFormat::D16_UNORMAL_S8_UINT:					return VK_FORMAT_D16_UNORM_S8_UINT;
				case PixelFormat::D24_UNORMAL_S8_UINT:					return VK_FORMAT_D24_UNORM_S8_UINT;
				case PixelFormat::D32_SFLOAT_S8_UINT:					return VK_FORMAT_D32_SFLOAT_S8_UINT;
				default:												throw Flint::BackendError("Invalid pixel format!");
				}

				return VK_FORMAT_UNDEFINED;
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
		}
	}
}