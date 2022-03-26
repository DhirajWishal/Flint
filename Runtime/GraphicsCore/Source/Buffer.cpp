// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/Buffer.hpp"

namespace Flint
{
	Buffer::Buffer(const std::shared_ptr<Device>& pDevice, const BufferType type, const uint64 size, const BufferMemoryProfile profile)
		: DeviceBoundObject(pDevice), mType(type), mSize(size), mMemoryProfile(profile)
	{
		if (type == BufferType::Undefined)
			throw std::invalid_argument("Buffer type should not be Undefined!");

		if (size == 0)
			throw std::invalid_argument("Buffer size should be greater than 0!");
	}
}