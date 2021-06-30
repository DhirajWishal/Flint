// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "StaggingBuffer.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint draw resource buffer object.
		 * This buffer types is used to submit data to the pipeline at draw time. Index and Vertex data are stored in these buffers.
		 * These buffers cannot be mapped to the local address space, but data needs to be transfered using a stagging buffer.
		 */
		class DrawResourceBuffer : public Buffer
		{
		public:
			DrawResourceBuffer(const std::shared_ptr<Device>& pDevice, UI64 size) : Buffer(pDevice, size) {}

			/**
			 * Get the stride of the buffer.
			 *
			 * @return The stride in bytes.
			 */
			virtual UI64 GetStride() const = 0;
		};
	}
}