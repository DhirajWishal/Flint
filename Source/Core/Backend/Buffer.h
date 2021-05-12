// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Buffer object.
		 * This object represents a block of memory in the GPU and data can be copied to it.
		 */
		class Buffer : public DeviceBoundObject
		{
			struct PreviousMemoryMapInfo
			{
				UI64 mSize = 0;
				UI64 mOffset = 0;
			};

		public:
			/**
			 * Construct the buffer object.
			 *
			 * @param pDevice: The device object to which the buffer is bound to.
			 * @param size: The size of the buffer.
			 * @param usage: The usage of the buffer.
			 * @param memoryProfile: The memory profile of the buffer.
			 */
			Buffer(Device* pDevice, UI64 size, BufferUsage usage, MemoryProfile memoryProfile)
				: DeviceBoundObject(pDevice), mSize(size), mUsage(usage), mMemoryProfile(memoryProfile) {}

			/**
			 * Map memory from the device address space to the local address space.
			 *
			 * @param size: The size to be mapped.
			 * @param offset: The offset of the memory to be mapped.
			 * @return The mapped memory pointer.
			 */
			virtual void* MapMemory(UI64 size, UI64 offset) = 0;

			/**
			 * Flush the mapped memory range.
			 * This step is recommended to make sure that the copied data will be pushed to the device.
			 */
			virtual void FlushMemoryMappings() = 0;

			/**
			 * Unmap the mapped memory range.
			 */
			virtual void UnmapMemory() = 0;

			/**
			 * Copy data to this buffer using another buffer.
			 *
			 * @param pBuffer: The buffer to copy data from.
			 * @param size: The size of the source buffer to copy.
			 * @param srcOffset: The offset of the source buffer to copy from.
			 * @param dstOffset: The size of the destination buffer (this) to copy data to.
			 */
			virtual void CopyFrom(const Buffer* pBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset) = 0;

			/**
			 * Get the size of the buffer.
			 *
			 * @return The size.
			 */
			UI64 GetSize() const { return mSize; }

			/**
			 * Get the buffer usage.
			 *
			 * @return The usage.
			 */
			BufferUsage GetUsage() const { return mUsage; }

			/**
			 * Get the memory profile of the buffer.
			 *
			 * @return The memory profile.
			 */
			MemoryProfile GetMemoryProfile() const { return mMemoryProfile; }

		protected:
			PreviousMemoryMapInfo mPrevMapInfo = {};

			UI64 mSize = 0;

			BufferUsage mUsage = BufferUsage::UNDEFINED;
			MemoryProfile mMemoryProfile = MemoryProfile::TRANSFER_FRIENDLY;
		};
	}
}