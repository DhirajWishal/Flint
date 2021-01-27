// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "LowLevel/Device.h"

namespace Flint
{
	namespace LowLevel
	{
		/**
		 * Flint Vertex Buffer object.
		 * Vertex buffers are special buffers in which the vertex geometry is passed to the pipeline.
		 */
		class VertexBuffer {
		public:
			VertexBuffer() {}
			~VertexBuffer() {}

			/**
			 * Initialize the vertex buffer using the vertex size and the vertex count.
			 *
			 * @param device: The device to which the buffer is bound to.
			 * @param vertexSize: The size of a single vertex.
			 * @param vertexCount: The number of vertexes the buffer will contain.
			 */
			void Initialize(const Device& device, UI64 vertexSize, UI64 vertexCount);

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
			Interface::VertexBufferHandle GetHandle() const { return mHandle; }

		private:
			UI64 mVertexSize = 0;
			UI64 mVertexCount = 0;
			Interface::VertexBufferHandle mHandle = {};
		};
	}
}