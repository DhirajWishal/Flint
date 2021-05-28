// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/FBufferManager.h"

namespace Flint
{
	namespace Templates
	{
		/**
		 * Flint buffer template.
		 */
		class Buffer
		{
		public:
			Buffer(UI64 size, BufferUsage usage, MemoryProfile memoryProfile) : mSize(size), mUsage(usage), mMemoryProfile(memoryProfile) {}

			UI64 GetSize() const { return mSize; }
			BufferUsage GetUsage() const { return mUsage; }
			MemoryProfile GetMemoryProfile() const { return mMemoryProfile; }

		protected:
			UI64 mSize = 0;
			BufferUsage mUsage = BufferUsage::UNDEFINED;
			MemoryProfile mMemoryProfile = MemoryProfile::TRANSFER_FRIENDLY;
		};
	}
}