// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Buffer class.
	 * This class is used to store information about a single pool of memory.
	 */
	class Buffer : public DeviceBoundObject
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
		explicit Buffer(const std::shared_ptr<Device>& pDevice, uint64_t size, BufferUsage usage, const std::byte* pDataStore = nullptr);

		/**
		 * Default virtual destructor.
		 */
		virtual ~Buffer() = default;

		/**
		 * Map the buffer memory to the local address space.
		 *
		 * @return The byte pointer.
		 */
		[[nodiscard]] virtual std::byte* mapMemory() = 0;

		/**
		 * Unmap the memory from the local address space.
		 */
		virtual void unmapMemory() = 0;

		/**
		 * Copy data from a raw data pointer.
		 *
		 * @param pData The data pointer.
		 * @param size The size of the data to be copied.
		 * @param srcOffset The data source's offset.
		 * @param dstOffset The data destination's (this) offset.
		 */
		virtual void copyFrom(const std::byte* pData, uint64_t size, uint64_t srcOffset = 0, uint64_t dstOffset = 0) = 0;

		/**
		 * Copy content from another buffer to this.
		 *
		 * @param pBuffer The other buffer to copy from.
		 * @param srcOffset The offset of the source buffer to copy from.
		 * @param dstOffset The offset of the destination (this) buffer to copy to.
		 */
		virtual void copyFrom(const Buffer* pBuffer, uint64_t srcOffset = 0, uint64_t dstOffset = 0) = 0;

		/**
		 * Get the buffer's size.
		 *
		 * @return The size.
		 */
		[[nodiscard]] uint64_t getSize() const { return m_Size; }

		/**
		 * Get the buffer's usage.
		 *
		 * @return The usage.
		 */
		[[nodiscard]] BufferUsage getUsage() const { return m_Usage; }

	protected:
		const uint64_t m_Size;
		const BufferUsage m_Usage;
	};
}