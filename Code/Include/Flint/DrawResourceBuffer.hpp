// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "StaggingBuffer.hpp"

namespace Flint
{
	/**
	 * Flint draw resource buffer object.
	 * This buffer types is used to submit data to the pipeline at draw time. Index and Vertex data are stored in these buffers.
	 * These buffers cannot be mapped to the local address space, but data needs to be transfered using a stagging buffer.
	 */
	class FLINT_API DrawResourceBuffer : public Buffer
	{
	public:
		DrawResourceBuffer(Device& device, UI64 size) : Buffer(device, size) {}

		/**
		 * Copy data to this buffer using another buffer.
		 *
		 * @param srcBuffer: The source stagging buffer.
		 * @param size: The copy size.
		 * @param srcOffet: The source buffer offset.
		 * @param dstOffset: The destination (this) buffer offset.
		 */
		virtual void CopyFromBuffer(const StaggingBuffer& srcBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset) = 0;

		/**
		 * Get the stride of the buffer.
		 * 
		 * @return The stride in bytes.
		 */
		virtual UI64 GetStride() const = 0;
	};
}