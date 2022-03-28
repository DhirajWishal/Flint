// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Buffer resize mode enum.
	 * This defines what to do to the stored data after resizing.
	 */
	enum class BufferResizeMode : uint8_t
	{
		Clear,
		Copy,
	};

	/**
	 * Flint buffer object.
	 * Buffers are used to store data in the device.
	 */
	template<class DerivedT, class DeviceT>
	class Buffer : public DeviceBoundObject<DeviceT>
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param type The buffer type.
		 * @param size The size of the buffer.
		 * @param profile The memory profile of the buffer. Default is BufferMemoryProfile::Automatic.
		 */
		explicit Buffer(DeviceT* pDevice, const BufferType type, const uint64_t size, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic)
			: DeviceBoundObject(pDevice), m_Type(type), m_Size(size), m_MemoryProfile(profile)
		{
			if (type == BufferType::Undefined)
				throw std::invalid_argument("Buffer type should not be Undefined!");

			if (size == 0)
				throw std::invalid_argument("Buffer size should be greater than 0!");
		}

		/**
		 * Resize the buffer.
		 * Once resized, the mode applied will be placed, either the existing content will be cleared, or will bring the old content with it.
		 *
		 * @param size The new size of the buffer.
		 * @param mode The resize mode.
		 */
		virtual void Resize(const uint64_t size, const BufferResizeMode mode) = 0;

		/**
		 * Copy data to this buffer using another buffer.
		 *
		 * @param pSrcBuffer The source buffer.
		 * @param size The copy size.
		 * @param srcOffet The source buffer offset.
		 * @param dstOffset The destination (this) buffer offset.
		 */
		virtual void CopyFromBuffer(const DerivedT* pSrcBuffer, const uint64_t size, const uint64_t srcOffset, const uint64_t dstOffset) = 0;

		/**
		 * Map the buffer memory to the local address space.
		 *
		 * @param size The size to map.
		 * @param offset The offset of the buffer to map.
		 * @return The mapped address pointer.
		 */
		virtual void* MapMemory(const uint64_t size, const uint64_t offset = 0) = 0;

		/**
		 * Unmap the buffer memory from the local address space.
		 */
		virtual void UnmapMemory() = 0;

	public:
		/**
		 * Utility function supporting data copy.
		 * Make sure that the memory profile is transfer friendly or the buffer type supports data copy.
		 *
		 * @tparam Type The type of the data to be copied.
		 * @param data The data to copy.
		 * @param offset The offset in the buffer to copy data to. Default is 0.
		 */
		template<class Type>
		void CopyData(const Type& data, const uint64_t offset = 0)
		{
			Type* pData = static_cast<Type*>(MapMemory(sizeof(Type), offset));
			(*pData) = data;
			UnmapMemory();
		}

		/**
		 * Extend the buffer's size.
		 *
		 * @param sizeToAdd The size to extend the buffer with.
		 * @param mode The resize mode.
		 */
		void Extend(const uint64_t sizeToAdd, const BufferResizeMode mode) { Resize(GetSize() + sizeToAdd, mode); }

		/**
		 * Shrink the buffer's size.
		 *
		 * @param sizeToReduce The size to shrink the buffer with.
		 * @param mode The resize mode.
		 */
		void Shrink(const uint64_t sizeToReduce, const BufferResizeMode mode) { Resize(GetSize() - sizeToReduce, mode); }

		/**
		 * Get the buffer size.
		 *
		 * @return The size in bytes.
		 */
		const uint64_t GetSize() const { return m_Size; }

		/**
		 * Get the buffer type.
		 *
		 * @return The buffer type.
		 */
		const BufferType GetType() const { return m_Type; }

		/**
		 * Get the memory profile of the buffer.
		 *
		 * @return The memory profile.
		 */
		const BufferMemoryProfile GetMemoryProfile() const { return m_MemoryProfile; }

		/**
		 * Check if the buffer is already mapped.
		 *
		 * @return The boolean value.
		 */
		bool IsMapped() const { return bIsMapped; }

	protected:
		uint64_t m_Size = 0;
		BufferType m_Type = BufferType::Undefined;
		BufferMemoryProfile m_MemoryProfile = BufferMemoryProfile::Automatic;

		bool bIsMapped = false;
	};
}