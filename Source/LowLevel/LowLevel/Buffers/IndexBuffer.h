// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "LowLevel/Device.h"

namespace Flint
{
	namespace LowLevel
	{
		/**
		 * Flint Index Buffer object.
		 * Index buffers store grometry indexes.
		 */
		class IndexBuffer {
		public:
			IndexBuffer() {}
			~IndexBuffer() {}

			/**
			 * Initialize the index buffer using the index size and the index count.
			 *
			 * @param device: The device to which the buffer is bound to.
			 * @param vertexSize: The size of a single index.
			 * @param vertexCount: The number of indexes the buffer will contain.
			 */
			void Initialize(const Device& device, UI64 indexSize, UI64 indexCount);

			/**
			 * Terminate the buffer.
			 */
			void Terminate();

			/**
			 * Map the buffer's device memory space to the local address space.
			 * After this, you can copy/ move content to the buffer.
			 *
			 * @param size: The size of the memory block to be mapped.
			 * @param offset: The offset of the memor block to be mapped.
			 * @return The address of the mapped memory.
			 */
			void* MapMemory(UI64 size, UI64 offset);

			/**
			 * Unmap the mapped address space.
			 */
			void UnmapMemory();

		public:
			Interface::IndexBufferHandle GetHandle() const { return mHandle; }

		private:
			UI64 mIndexSize = 0;
			UI64 mIndexCount = 0;
			Interface::IndexBufferHandle mHandle = FLINT_NULL_HANDLE;
		};
	}
}