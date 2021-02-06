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
			VERTEX,
			INDEX,
			UNIFORM,
			STAGGING
		};

		/**
		 * Buffer object.
		 * This object is used to store information in the GPU.
		 */
		class Buffer : public BackendObject {
		public:
			Buffer() {}

			/**
			 * Initialize the buffer.
			 *
			 * @param pDevice: The device poitner to which the buffer is bound to.
			 * @param size: The size of the buffer.
			 * @param usage: The buffer usage.
			 * @param memoryProfile: The memory profile used by the buffer.
			 */
			virtual void Initialize(Device* pDevice, UI64 size, BufferUsage usage, MemoryProfile memoryProfile) = 0;

			/**
			 * Terminate the buffer.
			 */
			virtual void Terminate() = 0;

		public:
			Device* GetDevice() const { return pDevice; }
			UI64 GetSize() const { return mSize; }

			BufferUsage GetUsage() const { return mUsage; }
			MemoryProfile GetMemoryProfile() const { return mMemoryProfile; }

		protected:
			Device* pDevice = nullptr;
			UI64 mSize = 0;

			BufferUsage mUsage = BufferUsage::UNDEFINED;
			MemoryProfile mMemoryProfile = MemoryProfile::TRANSFER_FRIENDLY;
		};
	}
}