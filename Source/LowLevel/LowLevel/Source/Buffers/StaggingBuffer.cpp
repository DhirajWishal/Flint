// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LowLevel/Buffers/StaggingBuffer.h"
#include "Core/Backend/Interface/Buffer.h"
#include "Core/ErrorHandler/Logger.h"

namespace Flint
{
	namespace LowLevel
	{
		void StaggingBuffer::Initialize(const Device& device, UI64 size)
		{
			mSize = size;
			mHandle = Interface::CreateStaggingBuffer(device.GetHandle(), size);
		}

		void StaggingBuffer::Terminate()
		{
			Interface::DestroyStaggingBuffer(mHandle);
		}

		void* StaggingBuffer::MapMemory(UI64 size, UI64 offset)
		{
			if ((size + offset) > mSize || !size)
			{
				FLINT_LOG_ERROR(TEXT("Size and/ or Offset is invalid!"));
				return nullptr;
			}

			return Interface::MapStaggingBufferMemory(mHandle, size, offset);
		}

		void StaggingBuffer::UnmapMemory()
		{
			Interface::UnmapStaggingBufferMemory(mHandle);
		}
	}
}
