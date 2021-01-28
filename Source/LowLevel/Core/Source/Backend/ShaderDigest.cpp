// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Backend/ShaderDigest.h"
#include "Core/Backend/Interface/Buffer.h"

namespace Flint
{
	UniformBufferStorage ShaderDigest::CreatUniformBuffers(const Interface::DeviceHandle& deviceHandle)
	{
		UniformBufferStorage storage = {};

		for (auto itr = mUniforms.begin(); itr != mUniforms.end(); itr++)
		{
			UniformType type = itr->mType;
			if (type == UniformType::UNIFORM_BUFFER ||
				type == UniformType::UNIFORM_BUFFER_DYNAMIC ||
				type == UniformType::STORAGE_BUFFER ||
				type == UniformType::STORAGE_BUFFER_DYNAMIC)
				storage[itr->mName] = Interface::CreateUniformBuffer(deviceHandle, itr->mSize);
		}

		return storage;
	}
}