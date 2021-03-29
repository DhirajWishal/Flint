// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "Buffer.h"

namespace Flint
{
	namespace Backend
	{
		template<class DeviceType, class BufferType>
		class CommandBufferList : public BackendObject {
		public:
			using DeviceType = DeviceType;
			using BufferType = BufferType;

		public:
			CommandBufferList() {}
			virtual ~CommandBufferList() {}

			virtual void Initialize(DeviceType* pDevice, UI64 bufferCount) = 0;
			virtual void Terminate() = 0;

			virtual void BeginBuffer(UI64 index) = 0;
			virtual void BeginNextBuffer() { BeginBuffer(IncrementIndex()); }
			virtual void EndBuffer() = 0;

			virtual void BindVertexBuffer(const BufferType& buffer, UI64 firstBinding, UI64 bindingCount) = 0;
		public:
			UI64 GetBufferCount() const { return mBufferCount; }
			UI64 GetIndex() const { return mBufferIndex; }

			UI64 IncrementIndex()
			{
				mBufferIndex++;
				if (mBufferIndex >= mBufferCount) mBufferIndex = 0;
				return mBufferIndex;
			}

		protected:
			DeviceType* pDevice = nullptr;
			UI64 mBufferCount = 0;
			UI64 mBufferIndex = 0;
		};
	}
}