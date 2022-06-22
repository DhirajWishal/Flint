// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Buffer.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan buffer class.
		 */
		class VulkanBuffer final : public Core::Buffer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device to which the buffer is bound to.
			 * @param size The size of the buffer.
			 * @param usage The buffer usage.
			 */
			explicit VulkanBuffer(const std::shared_ptr<VulkanDevice>& pDevice, uint64_t size, Core::BufferUsage usage);

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
			 * Copy content from another buffer to this.
			 *
			 * @param buffer The other buffer to copy from.
			 * @param srcOffset The offset of the source buffer to copy from.
			 * @param dstOffset The offset of the destination (this) buffer to copy to.
			 */
			void copyFrom(const Core::Buffer& buffer, uint64_t srcOffset = 0, uint64_t dstOffset = 0) override;

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

			VkBuffer m_Buffer = VK_NULL_HANDLE;
			VmaAllocation m_Allocation = nullptr;

			bool m_IsMapped = false;
		};
	}
}