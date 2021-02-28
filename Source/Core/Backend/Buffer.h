// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBuffer.h"
#include "Device.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Buffer object.
		 * This object is used to store information in the GPU.
		 */
		class Buffer : public BackendObject {
			struct MemoryMapInfo {
				UI64 mSize = 0;
				UI64 mOffset = 0;
			};

		public:
			Buffer() {}

			/**
			 * Initialize the buffer.
			 *
			 * @param pDevice: The device pointer to which the buffer is bound to.
			 * @param size: The size of the buffer.
			 * @param usage: The buffer usage.
			 * @param memoryProfile: The memory profile used by the buffer.
			 */
			virtual void Initialize(Device* pDevice, UI64 size, BufferUsage usage, MemoryProfile memoryProfile) = 0;

			/**
			 * Terminate the buffer.
			 */
			virtual void Terminate() = 0;

			/**
			 * Map the buffer memory to the local address space.
			 * 
			 * @param size: The size to be mapped.
			 * @param offset: The offset of the buffer to be mapped.
			 * @return The mapped address pointer.
			 */
			virtual void* MapMemory(UI64 size, UI64 offset) = 0;

			/**
			 * Unmap the mapped memory from the local address space.
			 */
			virtual void UnmapMemory() = 0;

			/**
			 * Flush memory mappings to write data to the device.
			 */
			virtual void FlushMemoryMappings() = 0;

			/**
			 * Copy content from one buffer to this.
			 * 
			 * @param pBuffer: The other buffer pointer.
			 * @param size: The number of bytes to be copied.
			 * @param srcOffset: The offset of the source buffer to be copied from.
			 * @param dstOffset: The offset of the destination buffer to be copied to.
			 */
			virtual void CopyFrom(Buffer* pBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset) = 0;

			/**
			 * Bind a buffer to the command buffer.
			 * For draw calls, only Index and Vertex buffers are allowed to be bound.
			 * 
			 * @param pCommandBuffer: The command buffer to be bound to.
			 */
			virtual void Bind(const std::shared_ptr<CommandBuffer>& pCommandBuffer) = 0;

		public:
			/**
			 * Reset the buffer content back to its initial state.
			 * This does not alter the buffer itself, but the buffer memory.
			 */
			void Reset();

		public:
			Device* GetDevice() const { return pDevice; }
			UI64 GetSize() const { return mSize; }

			BufferUsage GetUsage() const { return mUsage; }
			MemoryProfile GetMemoryProfile() const { return mMemoryProfile; }

		protected:
			MemoryMapInfo mPrevMapInfo = {};

			Device* pDevice = nullptr;
			UI64 mSize = 0;

			BufferUsage mUsage = BufferUsage::UNDEFINED;
			MemoryProfile mMemoryProfile = MemoryProfile::TRANSFER_FRIENDLY;
		};
	}
}