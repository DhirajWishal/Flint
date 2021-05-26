// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint Stagging Buffer object.
		 * These are simple buffers which serves the purpose of transferring data to and between buffers and the GPU.
		 */
		class StaggingBuffer : public Buffer
		{
		public:
			StaggingBuffer(Device* pDevice, UI64 size) : Buffer(pDevice, size, BufferUsage::STAGGING, MemoryProfile::TRANSFER_FRIENDLY) {}
		};
	}
}