// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LowLevel/Buffers/VertexBuffer.h"
#include "Core/Backend/Interface/Buffer.h"
#include "Core/ErrorHandler/Logger.h"

namespace Flint
{
	namespace LowLevel
	{
		void VertexBuffer::Initialize(const Device& device, UI64 vertexSize, UI64 vertexCount)
		{
			mVertexSize = vertexSize;
			mVertexCount = vertexCount;
			mHandle = Interface::CreateVertexBuffer(device.GetHandle(), mVertexSize * mVertexCount);
		}

		void VertexBuffer::Terminate()
		{
			Interface::DestroyVertexBuffer(mHandle);
		}

		void* VertexBuffer::MapMemory(UI64 size, UI64 offset)
		{
			if ((size + offset) > (mVertexSize * mVertexCount) || !size)
			{
				FLINT_LOG_ERROR(TEXT("Size and/ or Offset is invalid!"));
				return nullptr;
			}

			return Interface::MapVertexBufferMemory(mHandle, size, offset);
		}

		void VertexBuffer::UnmapMemory()
		{
			Interface::UnmapVertexBufferMemory(mHandle);
		}
	}
}