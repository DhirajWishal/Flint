// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "LowLevel/Device.h"

namespace Flint
{
	namespace LowLevel
	{
		/**
		 * Flint Uniform Buffer object.
		 * Uniform buffers are used to submit resources to the shaders to be computed.
		 */
		class UniformBuffer {
		public:
			UniformBuffer() {}
			~UniformBuffer() {}

			/**
			 * Initialize the uniform buffer using the size.
			 *
			 * @param device: The device to which the unform buffer is bound to.
			 * @param size: The size of the buffer in bytes.
			 */
			void Initialize(const Device& device, UI64 size);

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
			Interface::UniformBufferHandle GetHandle() const { return mHandle; }

		private:
			UI64 mSize = 0;
			Interface::UniformBufferHandle mHandle = {};
		};
	}
}