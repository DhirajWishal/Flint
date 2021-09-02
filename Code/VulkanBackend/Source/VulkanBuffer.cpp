// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanOneTimeCommandBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanBuffer::VulkanBuffer(const std::shared_ptr<Device>& pDevice, const BufferType type, const UI64 size, const BufferMemoryProfile profile)
			: Buffer(pDevice, type, size, profile)
		{
			FLINT_SETUP_PROFILER();

			switch (type)
			{
			case Flint::BufferType::Staging:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vMemoryProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
				break;

			case Flint::BufferType::Vertex:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
				vMemoryProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				break;

			case Flint::BufferType::Index:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
				vMemoryProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				break;

			case Flint::BufferType::Uniform:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
				vMemoryProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
				break;

			case Flint::BufferType::Storage:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
				vMemoryProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
				break;

			default:
				throw std::range_error("Invalid or Undefined buffer type!");
				break;
			}

			switch (profile)
			{
			case Flint::BufferMemoryProfile::Automatic:
				break;

			case Flint::BufferMemoryProfile::CPUOnly:
			case Flint::BufferMemoryProfile::TransferFriendly:
				vMemoryProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
				break;

			case Flint::BufferMemoryProfile::DeviceOnly:
				vMemoryProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				break;

			default:
				throw backend_error("Invalid buffer memory profile type!");
			}

			CreateBuffer();
			CreateBufferMemory();
		}

		void VulkanBuffer::Resize(const UI64 size, const BufferResizeMode mode)	// TODO
		{
			FLINT_SETUP_PROFILER();
			UI64 oldSize = mSize;

			if (mode == BufferResizeMode::Copy)
			{
				// Create a stagging buffer to copy data to.
				std::shared_ptr<Buffer> pStagingBuffer = pDevice->CreateBuffer(BufferType::Staging, oldSize);
				pStagingBuffer->CopyFromBuffer(this->shared_from_this(), oldSize, 0, 0);

				// Terminate the existing buffer and get the new size.
				Terminate();
				mSize = size;

				// Create the new buffer.
				CreateBuffer();
				CreateBufferMemory();

				// Copy buffer content.
				CopyFromBuffer(pStagingBuffer, oldSize, 0, 0);
				pStagingBuffer->Terminate();
			}
			else if (mode == BufferResizeMode::Clear)
			{
				// Terminate the existing buffer and get the new size.
				Terminate();
				mSize += size;

				// Create the new buffer.
				CreateBuffer();
				CreateBufferMemory();
			}
			else
				throw std::invalid_argument("Buffer copy mode is invalid or undefined!");
		}

		void VulkanBuffer::CopyFromBuffer(const std::shared_ptr<Buffer>& pSrcBuffer, const UI64 size, const UI64 srcOffset, const UI64 dstOffset)
		{
			FLINT_SETUP_PROFILER();

			VkBufferCopy vBufferCopy = {};
			vBufferCopy.size = size;
			vBufferCopy.srcOffset = srcOffset;
			vBufferCopy.dstOffset = dstOffset;

			VulkanOneTimeCommandBuffer vCommandBuffer(pDevice->StaticCast<VulkanDevice>());
			vkCmdCopyBuffer(vCommandBuffer, pSrcBuffer->StaticCast<VulkanBuffer>().vBuffer, vBuffer, 1, &vBufferCopy);
		}

		void* VulkanBuffer::MapMemory(const UI64 size, const UI64 offset)
		{
			FLINT_SETUP_PROFILER();

			if (size + offset > mSize)
				throw std::range_error("Submitted size and offset goes beyond the buffer dimensions!");
			else if (size <= 0)
				throw std::range_error("Submitted size is invalid!");

			void* pDataStore = nullptr;
			FLINT_VK_ASSERT(vkMapMemory(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), vMemory, offset, size, 0, &pDataStore));

			return pDataStore;
		}

		void VulkanBuffer::UnmapMemory()
		{
			FLINT_SETUP_PROFILER();

			vkUnmapMemory(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), vMemory);
		}

		void VulkanBuffer::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vkDestroyBuffer(vDevice.GetLogicalDevice(), vBuffer, nullptr);
			vkFreeMemory(vDevice.GetLogicalDevice(), vMemory, nullptr);

			bIsTerminated = true;
		}

		void VulkanBuffer::CreateBuffer()
		{
			FLINT_SETUP_PROFILER();

			VkBufferCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			vCreateInfo.flags = 0;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.queueFamilyIndexCount = 0;
			vCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
			vCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			vCreateInfo.size = static_cast<UI32>(mSize);
			vCreateInfo.usage = vBufferUsage;

			FLINT_VK_ASSERT(vkCreateBuffer(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), &vCreateInfo, nullptr, &vBuffer));
		}

		void VulkanBuffer::CreateBufferMemory()
		{
			FLINT_SETUP_PROFILER();

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();

			VkMemoryRequirements vMR = {};
			vkGetBufferMemoryRequirements(vDevice.GetLogicalDevice(), vBuffer, &vMR);

			VkPhysicalDeviceMemoryProperties vMP = {};
			vkGetPhysicalDeviceMemoryProperties(vDevice.GetPhysicalDevice(), &vMP);

			VkMemoryAllocateInfo vAI = {};
			vAI.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			vAI.allocationSize = vMR.size;

			for (UI32 i = 0; i < vMP.memoryTypeCount; i++)
			{
				if ((vMR.memoryTypeBits & (1 << i)) && (vMP.memoryTypes[i].propertyFlags & vMemoryProperties) == vMemoryProperties)
				{
					vAI.memoryTypeIndex = i;
					break;
				}
			}

			FLINT_VK_ASSERT(vkAllocateMemory(vDevice.GetLogicalDevice(), &vAI, nullptr, &vMemory));
			FLINT_VK_ASSERT(vkBindBufferMemory(vDevice.GetLogicalDevice(), vBuffer, vMemory, 0));
		}
	}
}