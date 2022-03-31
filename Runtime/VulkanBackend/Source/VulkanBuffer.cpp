// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanOneTimeCommandBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanBuffer::VulkanBuffer(VulkanDevice* m_pDevice, const BufferType type, const uint64_t size, const BufferMemoryProfile profile)
			: Buffer(m_pDevice, type, size, profile)
		{
			OPTICK_EVENT();

			switch (type)
			{
			case Flint::BufferType::Staging:
				m_vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;

				vmaMemoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU;
				break;

			case Flint::BufferType::Vertex:
				m_vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

				vmaMemoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
				break;

			case Flint::BufferType::Index:
				m_vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

				vmaMemoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
				break;

			case Flint::BufferType::Uniform:
				m_vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

				vmaMemoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU;
				break;

			case Flint::BufferType::Storage:
				m_vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

				vmaMemoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU;
				break;

			default:
				throw std::range_error("Invalid or Undefined buffer type!");
			}

			switch (profile)
			{
			case Flint::BufferMemoryProfile::Automatic:
				break;

			case Flint::BufferMemoryProfile::CPUOnly:
				vmaMemoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_ONLY;
				break;

			case Flint::BufferMemoryProfile::TransferFriendly:
				vmaMemoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU;
				break;

			case Flint::BufferMemoryProfile::DeviceOnly:
				vmaMemoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
				break;

			default:
				throw backend_error("Invalid buffer memory profile type!");
			}

			CreateBuffer();
		}

		void VulkanBuffer::Resize(const uint64_t size, const BufferResizeMode mode)
		{
			OPTICK_EVENT();
			const uint64_t oldSize = m_Size;

			if (mode == BufferResizeMode::Copy)
			{
				// Create a stagging buffer to copy data to.
				auto pStagingBuffer = m_pDevice->CreateBuffer(BufferType::Staging, oldSize);
				pStagingBuffer->CopyFromBuffer(this, oldSize, 0, 0);

				// Terminate the existing buffer and get the new size.
				Terminate();
				m_Size = size;

				// Create the new buffer.
				CreateBuffer();

				// Copy buffer content.
				CopyFromBuffer(pStagingBuffer.get(), oldSize, 0, 0);
				pStagingBuffer->Terminate();
			}
			else if (mode == BufferResizeMode::Clear)
			{
				// Terminate the existing buffer and get the new size.
				Terminate();
				m_Size += size;

				// Create the new buffer.
				CreateBuffer();
			}
			else
				throw std::invalid_argument("Buffer copy mode is invalid or undefined!");
		}

		void VulkanBuffer::CopyFromBuffer(const VulkanBuffer* pSrcBuffer, const uint64_t size, const uint64_t srcOffset, const uint64_t dstOffset)
		{
			OPTICK_EVENT();

			VkBufferCopy vBufferCopy = {};
			vBufferCopy.size = size;
			vBufferCopy.srcOffset = srcOffset;
			vBufferCopy.dstOffset = dstOffset;

			VulkanOneTimeCommandBuffer vCommandBuffer(*m_pDevice);
			m_pDevice->GetDeviceTable().vkCmdCopyBuffer(vCommandBuffer, pSrcBuffer->StaticCast<VulkanBuffer>().m_vBuffer, m_vBuffer, 1, &vBufferCopy);
		}

		void* VulkanBuffer::MapMemory(const uint64_t size, const uint64_t offset)
		{
			OPTICK_EVENT();

			if (size + offset > m_Size)
				throw std::range_error("Submitted size and offset goes beyond the buffer dimensions!");

			else if (size <= 0)
				throw std::range_error("Submitted size is invalid!");

			void* pDataStore = nullptr;
			FLINT_VK_ASSERT(vmaMapMemory(m_pDevice->GetVmaAllocator(), vmaAllocation, &pDataStore));

			bIsMapped = true;
			return pDataStore;
		}

		void VulkanBuffer::UnmapMemory()
		{
			OPTICK_EVENT();

			vmaUnmapMemory(m_pDevice->GetVmaAllocator(), vmaAllocation);
			bIsMapped = false;
		}

		void VulkanBuffer::Terminate()
		{
			if (bIsMapped)
				UnmapMemory();

			vmaDestroyBuffer(m_pDevice->GetVmaAllocator(), m_vBuffer, vmaAllocation);

			bIsTerminated = true;
		}

		void VulkanBuffer::CreateBuffer()
		{
			OPTICK_EVENT();

			VkBufferCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			vCreateInfo.flags = 0;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.queueFamilyIndexCount = 0;
			vCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
			vCreateInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
			vCreateInfo.size = static_cast<uint32_t>(m_Size);
			vCreateInfo.usage = m_vBufferUsage;

			VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
			vmaAllocationCreateInfo.usage = vmaMemoryUsage;

			FLINT_VK_ASSERT(vmaCreateBuffer(m_pDevice->GetVmaAllocator(), &vCreateInfo, &vmaAllocationCreateInfo, &m_vBuffer, &vmaAllocation, nullptr));
		}
	}
}