// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

export module Flint.VulkanBackend.VulkanBuffer;

#include "GraphicsCore/Buffer.hpp"
import Flint.VulkanBackend.VulkanDevice;

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanBuffer final : public Buffer, public std::enable_shared_from_this<VulkanBuffer>
		{
		public:
			VulkanBuffer(const std::shared_ptr<Device>& pDevice, const BufferType type, const uint64 size, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic);
			~VulkanBuffer() { if (!bIsTerminated) Terminate(); }

			virtual void Resize(const uint64 size, const BufferResizeMode mode) override;
			virtual void CopyFromBuffer(const Buffer* pSrcBuffer, const uint64 size, const uint64 srcOffset, const uint64 dstOffset) override;
			virtual void* MapMemory(const uint64 size, const uint64 offset = 0) override;
			virtual void UnmapMemory() override;
			virtual void Terminate() override;

			VkBuffer GetBuffer() const { return vBuffer; }
			const VkBuffer* GetBufferAddress() const { return &vBuffer; }

		private:
			void CreateBuffer();

		private:
			VkBuffer vBuffer = VK_NULL_HANDLE;
			VmaAllocation vmaAllocation = {};

			VkBufferUsageFlags vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
			VmaMemoryUsage vmaMemoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_MAX_ENUM;
		};
	}
}

module: private;
#include "VulkanBackend/VulkanOneTimeCommandBuffer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanBuffer::VulkanBuffer(const std::shared_ptr<Device>& pDevice, const BufferType type, const uint64 size, const BufferMemoryProfile profile)
			: Buffer(pDevice, type, size, profile)
		{
			OPTICK_EVENT();

			switch (type)
			{
			case Flint::BufferType::Staging:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;

				vmaMemoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU;
				break;

			case Flint::BufferType::Vertex:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

				vmaMemoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
				break;

			case Flint::BufferType::Index:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

				vmaMemoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
				break;

			case Flint::BufferType::Uniform:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

				vmaMemoryUsage = VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU;
				break;

			case Flint::BufferType::Storage:
				vBufferUsage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

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

		void VulkanBuffer::Resize(const uint64 size, const BufferResizeMode mode)
		{
			OPTICK_EVENT();
			const uint64 oldSize = mSize;

			if (mode == BufferResizeMode::Copy)
			{
				// Create a stagging buffer to copy data to.
				std::shared_ptr<Buffer> pStagingBuffer = pDevice->CreateBuffer(BufferType::Staging, oldSize);
				pStagingBuffer->CopyFromBuffer(this, oldSize, 0, 0);

				// Terminate the existing buffer and get the new size.
				Terminate();
				mSize = size;

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
				mSize += size;

				// Create the new buffer.
				CreateBuffer();
			}
			else
				throw std::invalid_argument("Buffer copy mode is invalid or undefined!");
		}

		void VulkanBuffer::CopyFromBuffer(const Buffer* pSrcBuffer, const uint64 size, const uint64 srcOffset, const uint64 dstOffset)
		{
			OPTICK_EVENT();

			VkBufferCopy vBufferCopy = {};
			vBufferCopy.size = size;
			vBufferCopy.srcOffset = srcOffset;
			vBufferCopy.dstOffset = dstOffset;

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			VulkanOneTimeCommandBuffer vCommandBuffer(vDevice);
			vDevice.GetDeviceTable().vkCmdCopyBuffer(vCommandBuffer, pSrcBuffer->StaticCast<VulkanBuffer>().vBuffer, vBuffer, 1, &vBufferCopy);
		}

		void* VulkanBuffer::MapMemory(const uint64 size, const uint64 offset)
		{
			OPTICK_EVENT();

			if (size + offset > mSize)
				throw std::range_error("Submitted size and offset goes beyond the buffer dimensions!");

			else if (size <= 0)
				throw std::range_error("Submitted size is invalid!");

			void* pDataStore = nullptr;
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vmaMapMemory(vDevice.GetVmaAllocator(), vmaAllocation, &pDataStore));

			bIsMapped = true;
			return pDataStore;
		}

		void VulkanBuffer::UnmapMemory()
		{
			OPTICK_EVENT();

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vmaUnmapMemory(vDevice.GetVmaAllocator(), vmaAllocation);
			bIsMapped = false;
		}

		void VulkanBuffer::Terminate()
		{
			if (bIsMapped)
				UnmapMemory();

			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vmaDestroyBuffer(vDevice.GetVmaAllocator(), vBuffer, vmaAllocation);

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
			vCreateInfo.size = static_cast<uint32>(mSize);
			vCreateInfo.usage = vBufferUsage;

			VmaAllocationCreateInfo vmaAllocationCreateInfo = {};
			vmaAllocationCreateInfo.usage = vmaMemoryUsage;

			FLINT_VK_ASSERT(vmaCreateBuffer(pDevice->StaticCast<VulkanDevice>().GetVmaAllocator(), &vCreateInfo, &vmaAllocationCreateInfo, &vBuffer, &vmaAllocation, nullptr));
		}
	}
}