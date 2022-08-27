// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Backend/Buffer.hpp"
#include "VulkanDevice.hpp"

#include <mutex>

namespace Flint
{
	namespace Backend
	{
		class VulkanCommandBuffers;

		/**
		 * Vulkan buffer class.
		 */
		class VulkanBuffer final : public Buffer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device to which the buffer is bound to.
			 * @param size The buffer's size.
			 * @param usage The buffer's usage.
			 * @param pDataStore The data store pointer to copy everything from. Make sure that the raw buffer's size is the same or more than the buffer's size. Default is nullptr.
			 */
			explicit VulkanBuffer(const std::shared_ptr<VulkanDevice>& pDevice, uint64_t size, BufferUsage usage, const std::byte* pDataStore = nullptr);

			/**
			 * Destructor.
			 */
			~VulkanBuffer() override;

			/**
			 * Terminate the object.
			 */
			void terminate() override;

			/**
			 * Map the buffer memory to the local address space.
			 *
			 * @return The byte pointer.
			 */
			[[nodiscard]] std::byte* mapMemory() override;

			/**
			 * Unmap the memory from the local address space.
			 */
			void unmapMemory() override;

			/**
			 * Copy data from a raw data pointer.
			 *
			 * @param pData The data pointer.
			 * @param size The size of the data to be copied.
			 * @param srcOffset The data source's offset.
			 * @param dstOffset The data destination's (this) offset.
			 */
			void copyFrom(const std::byte* pData, uint64_t size, uint64_t srcOffset = 0, uint64_t dstOffset = 0) override;

			/**
			 * Copy content from another buffer to this.
			 *
			 * @param pBuffer The other buffer to copy from.
			 * @param srcOffset The offset of the source buffer to copy from.
			 * @param dstOffset The offset of the destination (this) buffer to copy to.
			 */
			void copyFrom(const Buffer* pBuffer, uint64_t srcOffset = 0, uint64_t dstOffset = 0) override;

			/**
			 * Copy content from another buffer to this.
			 * This will record the copy command and store it in the command buffer.
			 *
			 * @param pCommandBuffer The command buffer to record the command in.
			 * @param pBuffer The other buffer to copy from.
			 * @param srcOffset The data source's offset.
			 * @param dstOffset The data destination's (this) offset.
			 */
			void copyFromBatched(VulkanCommandBuffers* pCommandBuffer, const Buffer* pBuffer, uint64_t srcOffset = 0, uint64_t dstOffset = 0);

			/**
			 * Get the Vulkan buffer handle.
			 *
			 * @return The buffer.
			 */
			[[nodiscard]] VkBuffer getBuffer() const { return m_Buffer; }

			/**
			 * Get the descriptor buffer info pointer.
			 *
			 * @return The buffer info pointer.
			 */
			[[nodiscard]] const VkDescriptorBufferInfo* getDescriptorBufferInfo() const { return &m_DescriptorBufferInfo; }

		private:
			/**
			 * Create the buffer and validate this object.
			 */
			void createBufferAndValidate();

		private:
			std::mutex m_ResouceMutex;

			VkDescriptorBufferInfo m_DescriptorBufferInfo = {};

			VkBuffer m_Buffer = VK_NULL_HANDLE;
			VmaAllocation m_Allocation = nullptr;

			std::byte* m_pDataPointer = nullptr;

			bool m_IsMapped = false;
		};
	}
}