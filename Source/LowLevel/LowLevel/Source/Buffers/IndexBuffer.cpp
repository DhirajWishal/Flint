// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LowLevel/Buffers/IndexBuffer.h"
#include "Core/Backend/Interface/Buffer.h"
#include "Core/ErrorHandler/Logger.h"

namespace Flint
{
	namespace LowLevel
	{
		void IndexBuffer::Initialize(const Device& device, UI64 indexSize, UI64 indexCount)
		{
			mIndexSize = indexSize;
			mIndexCount = indexCount;
			mHandle = Interface::CreateIndexBuffer(device.GetHandle(), mIndexSize * mIndexCount);
		}

		void IndexBuffer::Terminate()
		{
			Interface::DestroyIndexBuffer(mHandle);
		}

		void* IndexBuffer::MapMemory(UI64 size, UI64 offset)
		{
			if ((size + offset) > (mIndexSize * mIndexCount) || !size)
			{
				FLINT_LOG_ERROR(TEXT("Size and/ or Offset is invalid!"));
				return nullptr;
			}

			return Interface::MapIndexBufferMemory(mHandle, size, offset);
		}

		void IndexBuffer::UnmapMemory()
		{
			Interface::UnmapIndexBufferMemory(mHandle);

		}
	}
}