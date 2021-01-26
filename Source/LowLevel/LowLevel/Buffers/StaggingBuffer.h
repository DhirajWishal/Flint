// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "LowLevel/Device.h"

namespace Flint
{
	namespace LowLevel
	{
		class StaggingBuffer {
		public:
			StaggingBuffer() {}
			~StaggingBuffer() {}

			void Initialize(const Device& device, UI64 size);
			void Terminate();

			void* MapMemory(UI64 size, UI64 offset);
			void UnmapMemory();

		public:
			Interface::StaggingBufferHandle GetHandle() const { return mHandle; }

		private:
			Interface::StaggingBufferHandle mHandle = {};
			UI64 mSize = 0;
		};
	}
}