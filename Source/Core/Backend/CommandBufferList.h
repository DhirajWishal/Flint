// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "Buffer.h"

namespace Flint
{
	namespace Backend
	{
		template<class Derived, class DeviceType, class BufferType>
		class CommandBufferList : public BackendObject<Derived> {
		public:
			using DeviceType = DeviceType;
			using BufferType = BufferType;

		public:
			CommandBufferList() {}
			virtual ~CommandBufferList() {}

			void Initialize(std::shared_ptr<DeviceType> pDevice, UI64 bufferCount) { this->pDevice = pDevice, this->mBufferCount = bufferCount; GetDerived().mInitialize(); }
			void Terminate() { GetDerived().mTerminate(); }

			void BeginBuffer(UI64 index) { this->mBufferIndex = index; GetDerived().mBeginBuffer(); }
			void BeginNextBuffer() { BeginBuffer(IncrementIndex()); }
			void EndBuffer() { GetDerived().mEndBuffer(); }

			void BindBuffer(const BufferType& buffer) { GetDerived().mBindBuffer(buffer); }
			void BindVertexBuffer(const BufferType& buffer, UI64 firstBinding, UI64 bindingCount) {}
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
			virtual void mInitialize() = 0;
			virtual void mTerminate() = 0;

			virtual void mBeginBuffer() = 0;
			virtual void mEndBuffer() = 0;

			virtual void mBindBuffer(const BufferType& buffer) = 0;
			virtual void mBindVertexBuffer(const BufferType& buffer, UI64 firstBinding, UI64 bindingCount) = 0;

			std::shared_ptr<DeviceType> pDevice = {};
			UI64 mBufferCount = 0;
			UI64 mBufferIndex = 0;
		};
	}
}