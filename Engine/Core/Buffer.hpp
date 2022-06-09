// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	namespace Core
	{
		/**
		 * Buffer usage enum.
		 */
		enum class BufferUsage : uint8_t
		{
			// Used to store vertex data. Note that in order to supply data to this type, we need a staging buffer.
			Vertex = 1 << 0,

			// Used to store index data. Note that in order to supply data to this type, we need a staging buffer.
			Index = 1 << 1,

			// Used to store vertex data. Note that unlike the other, this can directly receive data.
			ShallowVertex = 1 << 2,

			// Used to store index data. Note that unlike the other, this can directly receive data.
			ShallowIndex = 1 << 3,

			// Used to store uniform data.
			Uniform = 1 << 4,

			// Used to store data from the shader.
			Storage = 1 << 5,

			// Used for both uniform and storage purposes.
			General = Uniform | Storage,

			// Used for data transferring purposes.
			Staging = 1 << 6
		};

		/**
		 * Buffer class.
		 * This class is used to store information about a single pool of memory.
		 */
		template<class TDerived, class TDevice>
		class Buffer : public DeviceBoundObject<TDevice>
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device to which the buffer is bound to.
			 * @param size The buffer's size.
			 * @param usage The buffer's usage.
			 */
			explicit Buffer(TDevice& device, uint64_t size, BufferUsage usage) : DeviceBoundObject<TDevice>(device), m_Size(size), m_Usage(usage) {}

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
			virtual void copyFrom(const TDerived& buffer, uint64_t srcOffset = 0, uint64_t dstOffset = 0) = 0;

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