// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"

namespace Flint
{
	namespace Backend
	{
		enum class BufferUsage : UI8 {
			UNDEFINED,
			VERTEX, INDEX, STAGGING, UNIFORM
		};

		enum class MemoryProfile : UI8 {
			TRANSFER_FRIENDLY,
			DEVICE_ONLY,
			DRAW_RESOURCE
		};

		template<class TDerived, class TDevice>
		class Buffer : public BackendObject {
			struct PreviousMemoryMapInfo {
				UI64 mSize = 0;
				UI64 mOffset = 0;
			};

		public:
			using Derived = TDerived;
			using DeviceType = TDevice;

		public:
			Buffer() {}
			virtual ~Buffer() {}

			virtual void Initialize(DeviceType* pDevice, UI64 size, BufferUsage usage, MemoryProfile profile) = 0;
			virtual void Terminate() = 0;

			virtual void* MapMemory(UI64 size, UI64 offset) = 0;
			virtual void FlushMemoryMappings() = 0;
			virtual void UnmapMemory() = 0;

			virtual void CopyFrom(const Derived& buffer, UI64 size, UI64 srcOffset, UI64 dstOffset) = 0;

			DeviceType* GetDevice() const { return pDevice; }
			UI64 GetSize() const { return mSize; }
			BufferUsage GetUsage() const { return mUsage; }
			MemoryProfile GetMemoryProfile() const { return mMemoryProfile; }

		protected:
			PreviousMemoryMapInfo mPrevMapInfo = {};

			DeviceType* pDevice = {};
			UI64 mSize = 0;

			BufferUsage mUsage = BufferUsage::UNDEFINED;
			MemoryProfile mMemoryProfile = MemoryProfile::TRANSFER_FRIENDLY;
		};
	}
}