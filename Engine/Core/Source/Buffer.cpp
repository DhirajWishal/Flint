// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Buffer.hpp"
#include "Core/Errors/InvalidArgumentError.hpp"

namespace Flint
{
	Buffer::Buffer(const std::shared_ptr<Device>& pDevice, uint64_t size, BufferUsage usage, const std::byte* pDataStore /*= nullptr*/)
		: DeviceBoundObject(pDevice), m_Size(size), m_Usage(usage)
	{
		if (size == 0)
			throw InvalidArgumentError("The buffer's size should be grater than 0!");
	}
}