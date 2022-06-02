// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Buffer type enum.
		 */
		enum class BufferType : uint32_t
		{
			// Used to store vertex data. Note that in order to supply data to this type, we need a staging buffer.
			Vertex = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

			// Used to store index data. Note that in order to supply data to this type, we need a staging buffer.
			Index = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

			// Used to store vertex data. Note that unlike the other, this can directly receive data.
			ShallowVertex = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

			// Used to store index data. Note that unlike the other, this can directly receive data.
			ShallowIndex = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

			// Used to store uniform data.
			Uniform = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

			// Used to store data from the shader.
			Storage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

			// Used for both uniform and storage purposes.
			General = Uniform | Storage,

			// Used for data transferring purposes.
			Staging = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
		};

		/**
		 * Vulkan buffer class.
		 */
		class VulkanBuffer final
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device to which the buffer is bound to.
			 * @param size The size of the buffer.
			 * @param type The buffer type.
			 */
			explicit VulkanBuffer(VulkanDevice& device, uint64_t size, BufferType type);

			/**
			 * Destructor.
			 */
			~VulkanBuffer();

			/**
			 * Map the buffer memory to the local address space.
			 *
			 * @return The byte pointer.
			 */
			std::byte* mapMemory();

			/**
			 * Unmap the memory from the local address space.
			 */
			void unmapMemory();

			/**
			 * Copy content from another buffer to this.
			 *
			 * @param buffer The other buffer to copy from.
			 * @param srcOffset The offset of the source buffer to copy from.
			 * @param dstOffset The offset of the destination (this) buffer to copy to.
			 */
			void copyFrom(const VulkanBuffer& buffer, uint64_t srcOffset = 0, uint64_t dstOffset = 0);

			/**
			 * Get the size of the buffer.
			 *
			 * @return The size.
			 */
			[[nodiscard]] uint64_t getSize() const { return m_Size; }

			/**
			 * Get the type of the buffer.
			 *
			 * @return The buffer type.
			 */
			[[nodiscard]] BufferType getType() const { return m_Type; }

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
			VkDescriptorBufferInfo m_DescriptorBufferInfo = {};

			VulkanDevice& m_Device;

			VkBuffer m_Buffer = VK_NULL_HANDLE;
			VmaAllocation m_Allocation = nullptr;

			const uint64_t m_Size = 0;
			const BufferType m_Type = BufferType::Staging;

			bool m_IsMapped = false;
		};
	}
}