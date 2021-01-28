// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LowLevel/Device.h"
#include "Core/Backend/Interface/Device.h"

#include "LowLevel/Buffers/StaggingBuffer.h"
#include "LowLevel/Buffers/VertexBuffer.h"
#include "LowLevel/Buffers/IndexBuffer.h"
#include "LowLevel/Buffers/UniformBuffer.h"

namespace Flint
{
	namespace LowLevel
	{
		void Device::Initialize(const Display& display)
		{
			mHandle = Interface::CreateDevice(display.GetHandle());
		}

		void Device::Terminate()
		{
			Interface::DestroyDevice(mHandle);
		}

		StaggingBuffer Device::CreateStaggingBuffer(UI64 size)
		{
			StaggingBuffer buffer = {};
			buffer.Initialize(*this, size);

			return buffer;
		}

		VertexBuffer Device::CreateVertexBuffer(UI64 vertexSize, UI64 vertexCount)
		{
			VertexBuffer buffer = {};
			buffer.Initialize(*this, vertexSize, vertexCount);

			return buffer;
		}

		IndexBuffer Device::CreateIndexBuffer(UI64 indexSize, UI64 indexCount)
		{
			IndexBuffer buffer = {};
			buffer.Initialize(*this, indexSize, indexCount);

			return buffer;
		}

		UniformBuffer Device::CreateUniformBuffer(UI64 size)
		{
			UniformBuffer buffer = {};
			buffer.Initialize(*this, size);

			return buffer;
		}
	}
}