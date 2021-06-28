// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint transfer friendly buffer object.
		 * These buffers can be mapped to the local address space to transfer data.
		 */
		class TransferFriendlyBuffer : public Buffer
		{
		public:
			TransferFriendlyBuffer(Device& device, UI64 size) : Buffer(device, size) {}

			/**
			 * Map the buffer memory to the local address space.
			 *
			 * @param size: The size to map.
			 * @param offset: The offset of the buffer to map. Default is 0.
			 * @return The memory pointer.
			 */
			virtual void* MapMemory(UI64 size, UI64 offset = 0) = 0;

			/**
			 * Unmap the mapped memory from the local address space.
			 * This function will submit data to the device.
			 */
			virtual void UnmapMemory() = 0;
		};
	}
}