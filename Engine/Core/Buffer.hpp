// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	namespace Core
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
			 */
			explicit Buffer(const std::shared_ptr<Device>& pDevice, uint64_t size, BufferUsage usage) : DeviceBoundObject(pDevice), m_Size(size), m_Usage(usage) {}

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
			 * Copy content from another buffer to this.
			 *
			 * @param buffer The other buffer to copy from.
			 * @param srcOffset The offset of the source buffer to copy from.
			 * @param dstOffset The offset of the destination (this) buffer to copy to.
			 */
			virtual void copyFrom(const Buffer& buffer, uint64_t srcOffset = 0, uint64_t dstOffset = 0) = 0;

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
}