// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Buffer resize mode enum.
		 * This defines what to do to the stored data after resizing.
		 */
		enum class BufferResizeMode : UI8 {
			CLEAR,
			COPY,
		};

		/**
		 * Flint buffer object.
		 * Buffers are used to store data in the device.
		 */
		class Buffer : public DeviceBoundObject
		{
		public:
			Buffer(const std::shared_ptr<Device>& pDevice, const UI64 size) : DeviceBoundObject(pDevice), mSize(size) {}
			Buffer(const std::shared_ptr<Device>& pDevice, BufferType type, const UI64 size) : DeviceBoundObject(pDevice), mSize(size) {}

			/**
			 * Resize the buffer.
			 * Once resized, the mode applied will be placed, either the existing content will be cleared, or will bring the old content with it.
			 *
			 * @param size: The new size of the buffer.
			 * @param mode: The resize mode.
			 */
			virtual void Resize(UI64 size, BufferResizeMode mode) = 0;

			/**
			 * Copy data to this buffer using another buffer.
			 *
			 * @param srcBuffer: The source buffer.
			 * @param size: The copy size.
			 * @param srcOffet: The source buffer offset.
			 * @param dstOffset: The destination (this) buffer offset.
			 */
			virtual void CopyFromBuffer(const Buffer& srcBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset) = 0;

			/**
			 * Map the buffer memory to the local address space.
			 *
			 * @param size: The size to map.
			 * @param offset: The offset of the buffer to map.
			 * @return The mapped address pointer.
			 */
			virtual void* MapMemory(UI64 size, UI64 offset = 0) = 0;

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
			void Extend(UI64 sizeToAdd, BufferResizeMode mode) { Resize(GetSize() + sizeToAdd, mode); }

			/**
			 * Shrink the buffer's size.
			 *
			 * @param sizeToReduce: The size to shrink the buffer with.
			 * @param mode: The resize mode.
			 */
			void Shrink(UI64 sizeToReduce, BufferResizeMode mode) { Resize(GetSize() - sizeToReduce, mode); }

			/**
			 * Get the buffer size.
			 *
			 * @return The size in bytes.
			 */
			UI64 GetSize() const { return mSize; }

			/**
			 * Get the buffer type.
			 *
			 * @return The buffer type.
			 */
			BufferType GetType() const { return mType; }

		protected:
			UI64 mSize = 0;
			BufferType mType = BufferType::UNDEFINED;
		};
	}

	class BufferWrapper
	{
	public:
		BufferWrapper(Backend::Buffer& buffer) : mBuffer(buffer) {}

		/**
		 * Resize the buffer.
		 * Once resized, the mode applied will be placed, either the existing content will be cleared, or will bring the old content with it.
		 *
		 * @param size: The new size of the buffer.
		 * @param mode: The resize mode.
		 */
		void Resize(UI64 size, Backend::BufferResizeMode mode) { mBuffer.Resize(size, mode); }

		/**
		 * Copy data to this buffer using another buffer.
		 *
		 * @param srcBuffer: The source buffer.
		 * @param size: The copy size.
		 * @param srcOffet: The source buffer offset.
		 * @param dstOffset: The destination (this) buffer offset.
		 */
		void CopyFromBuffer(const BufferWrapper& srcBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset) { mBuffer.CopyFromBuffer(srcBuffer, size, srcOffset, dstOffset); }

		/**
		 * Map the buffer memory to the local address space.
		 *
		 * @param size: The size to map.
		 * @param offset: The offset of the buffer to map.
		 * @return The mapped address pointer.
		 */
		void* MapMemory(UI64 size, UI64 offset = 0) { return mBuffer.MapMemory(size, offset); }

		/**
		 * Unmap the buffer memory from the local address space.
		 */
		void UnmapMemory() { mBuffer.UnmapMemory(); }

	public:
		/**
		 * Extend the buffer's size.
		 *
		 * @param sizeToAdd: The size to extend the buffer with.
		 * @param mode: The resize mode.
		 */
		void Extend(UI64 sizeToAdd, Backend::BufferResizeMode mode) { Resize(GetSize() + sizeToAdd, mode); }

		/**
		 * Shrink the buffer's size.
		 *
		 * @param sizeToReduce: The size to shrink the buffer with.
		 * @param mode: The resize mode.
		 */
		void Shrink(UI64 sizeToReduce, Backend::BufferResizeMode mode) { Resize(GetSize() - sizeToReduce, mode); }

		/**
		 * Get the buffer size.
		 *
		 * @return The size in bytes.
		 */
		UI64 GetSize() const { return mBuffer.GetSize(); }

		/**
		 * Get the buffer type.
		 *
		 * @return The buffer type.
		 */
		Backend::BufferType GetType() const { return mBuffer.GetType(); }

	public:
		operator Backend::Buffer& () { return mBuffer; }
		operator const Backend::Buffer& () const { return mBuffer; }

		Backend::Buffer& operator->() { return mBuffer; }
		const Backend::Buffer& operator->() const { return mBuffer; }

		bool IsNull() const { return &mBuffer == nullptr; }

	protected:
		Backend::Buffer& mBuffer;
	};
}