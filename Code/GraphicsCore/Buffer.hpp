// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Buffer resize mode enum.
	 * This defines what to do to the stored data after resizing.
	 */
	enum class BufferResizeMode : UI8 {
		Clear,
		Copy,
	};

	/**
	 * Flint buffer object.
	 * Buffers are used to store data in the device.
	 */
	class Buffer : public DeviceBoundObject
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice: The device pointer.
		 * @param type: The buffer type.
		 * @param size: The size of the buffer.
		 * @param profile: The memory profile of the buffer. Default is BufferMemoryProfile::Automatic.
		 */
		Buffer(const std::shared_ptr<Device>& pDevice, const BufferType type, const UI64 size, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic);

		/**
		 * Resize the buffer.
		 * Once resized, the mode applied will be placed, either the existing content will be cleared, or will bring the old content with it.
		 *
		 * @param size: The new size of the buffer.
		 * @param mode: The resize mode.
		 */
		virtual void Resize(const UI64 size, const BufferResizeMode mode) = 0;

		/**
		 * Copy data to this buffer using another buffer.
		 *
		 * @param pSrcBuffer: The source buffer.
		 * @param size: The copy size.
		 * @param srcOffet: The source buffer offset.
		 * @param dstOffset: The destination (this) buffer offset.
		 */
		virtual void CopyFromBuffer(const Buffer* pSrcBuffer, const UI64 size, const UI64 srcOffset, const UI64 dstOffset) = 0;

		/**
		 * Map the buffer memory to the local address space.
		 *
		 * @param size: The size to map.
		 * @param offset: The offset of the buffer to map.
		 * @return The mapped address pointer.
		 */
		virtual void* MapMemory(const UI64 size, const UI64 offset = 0) = 0;

		/**
		 * Unmap the buffer memory from the local address space.
		 */
		virtual void UnmapMemory() = 0;

	public:
		/**
		 * Extend the buffer's size.
		 *
		 * @param sizeToAdd: The size to extend the buffer with.
		 * @param mode: The resize mode.
		 */
		void Extend(const UI64 sizeToAdd, const BufferResizeMode mode) { Resize(GetSize() + sizeToAdd, mode); }

		/**
		 * Shrink the buffer's size.
		 *
		 * @param sizeToReduce: The size to shrink the buffer with.
		 * @param mode: The resize mode.
		 */
		void Shrink(const UI64 sizeToReduce, const BufferResizeMode mode) { Resize(GetSize() - sizeToReduce, mode); }

		/**
		 * Get the buffer size.
		 *
		 * @return The size in bytes.
		 */
		const UI64 GetSize() const { return mSize; }

		/**
		 * Get the buffer type.
		 *
		 * @return The buffer type.
		 */
		const BufferType GetType() const { return mType; }

		/**
		 * Get the memory profile of the buffer.
		 *
		 * @return The memory profile.
		 */
		const BufferMemoryProfile GetMemoryProfile() const { return mMemoryProfile; }

	protected:
		UI64 mSize = 0;
		BufferType mType = BufferType::Undefined;
		BufferMemoryProfile mMemoryProfile = BufferMemoryProfile::Automatic;
	};
}