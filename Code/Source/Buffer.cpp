// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Buffer.hpp"

namespace Flint
{
	Buffer::Buffer(const std::shared_ptr<Device>& pDevice, BufferType type, const UI64 size, BufferMemoryProfile profile)
		: DeviceBoundObject(pDevice), mType(type), mSize(size), mMemoryProfile(profile)
	{
		if (type == BufferType::UNDEFINED)
			FLINT_THROW_INVALID_ARGUMENT("Buffer type should not be UNDEFINED!");

		if (size == 0)
			FLINT_THROW_INVALID_ARGUMENT("Buffer size should be greater than 0!");
	}
}