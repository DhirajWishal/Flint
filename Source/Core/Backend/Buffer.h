// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Buffer usage enum.
		 * Defines what the buffer will be used for,
		 */
		enum class BufferUsage : UI8 {
			UNDEFINED,
			VERTEX, INDEX, STAGGING, UNIFORM
		};

		class Buffer : public DeviceBoundObject
		{
			struct PreviousMemoryMapInfo
			{
				UI64 mSize = 0;
				UI64 mOffset = 0;
			};

		public:
			Buffer(Device* pDevice, UI64 size, BufferUsage usage, MemoryProfile memoryProfile)
				: DeviceBoundObject(pDevice), mSize(size), mUsage(usage), mMemoryProfile(memoryProfile) {}
			virtual ~Buffer() {}

			virtual void* MapMemory(UI64 size, UI64 offset) = 0;
			virtual void FlushMemoryMappings() = 0;
			virtual void UnmapMemory() = 0;
			virtual void CopyFrom(const Buffer* pBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset) = 0;

			UI64 GetSize() const { return mSize; }
			BufferUsage GetUsage() const { return mUsage; }
			MemoryProfile GetMemoryProfile() const { return mMemoryProfile; }

		protected:
			PreviousMemoryMapInfo mPrevMapInfo = {};

			UI64 mSize = 0;

			BufferUsage mUsage = BufferUsage::UNDEFINED;
			MemoryProfile mMemoryProfile = MemoryProfile::TRANSFER_FRIENDLY;
		};
	}
}