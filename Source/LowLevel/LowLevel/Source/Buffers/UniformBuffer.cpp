// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LowLevel/Buffers/UniformBuffer.h"
#include "Core/Backend/Interface/Buffer.h"
#include "Core/ErrorHandler/Logger.h"

namespace Flint
{
	namespace LowLevel
	{
		void UniformBuffer::Initialize(const Device& device, UI64 size)
		{
			mSize = size;
			mHandle = Interface::CreateUniformBuffer(device.GetHandle(), size);
		}

		void UniformBuffer::Terminate()
		{
			Interface::DestroyUniformBuffer(mHandle);
		}

		void* UniformBuffer::MapMemory(UI64 size, UI64 offset)
		{
			if ((size + offset) > mSize || !size)
			{
				FLINT_LOG_ERROR(TEXT("Size and/ or Offset is invalid!"));
				return nullptr;
			}

			return Interface::MapUniformBufferMemory(mHandle, size, offset);
		}

		void UniformBuffer::UnmapMemory()
		{
			Interface::UnmapUniformBufferMemory(mHandle);
		}
	}
}