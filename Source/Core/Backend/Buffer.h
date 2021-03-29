// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"

namespace Flint
{
	namespace Backend
	{
		enum class BufferType : UI8 {
			UNDEFINED,
			VERTEX, INDEX, STAGGING, UNIFORM
		};

		enum class MemoryProfile : UI8 {
			TRANSFER_FRIENDLY,
			DEVICE_ONLY,
			DRAW_RESOURCE
		};

		template<class Derived, class DeviceType>
		class Buffer : public BackendObject<Derived> {
			struct PreviousMemoryMapInfo {
				UI64 mSize = 0;
				UI64 mOffset = 0;
			};

		public:
			using DeviceType = DeviceType;

		public:
			Buffer() {}
			virtual ~Buffer() {}

			void Initialize(std::shared_ptr<DeviceType> pDevice, UI64 size, BufferType type, MemoryProfile profile)
			{
				this->pDevice = pDevice, this->mSize = size, this->mType = type, this->mMemoryProfile = profile;
				GetDerived().mInitialize();
			}

			void* MapMemory(UI64 size, UI64 offset) { return GetDerived().pMapMemory(size, offset); }
			void FlushMemoryMappings() { GetDerived().mFlushMemoryMappings(); }
			void UnmapMemory() { GetDerived().mUnmapMemory(); }

			void Terminate() { GetDerived().mTerminate(); }

			void CopyFrom(Buffer* pBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset) { GetDerived().mCopyFrom(pBuffer, size, srcOffset, dstOffset); }

			DeviceType* GetDevice() const { return pDevice.get(); }
			UI64 GetSize() const { return mSize; }
			BufferType GetType() const { return mType; }
			MemoryProfile GetMemoryProfile() const { return mMemoryProfile; }

		protected:
			virtual void mInitialize() = 0;
			virtual void* pMapMemory(UI64 size, UI64 offset) = 0;
			virtual void mFlushMemoryMappings() = 0;
			virtual void mUnmapMemory() = 0;
			virtual void mTerminate() = 0;

			virtual void mCopyFrom(Buffer* pBuffer, UI64 size, UI64 srcOffset, UI64 dstOffset) = 0;

			PreviousMemoryMapInfo mPrevMapInfo = {};

			std::shared_ptr<DeviceType> pDevice = {};
			UI64 mSize = 0;
			BufferType mType = BufferType::UNDEFINED;
			MemoryProfile mMemoryProfile = MemoryProfile::TRANSFER_FRIENDLY;
		};
	}
}