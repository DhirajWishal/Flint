// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/CommandBuffers.hpp"
#include "Core/Errors/InvalidArgumentError.hpp"

namespace Flint
{
	CommandBuffers::CommandBuffers(const std::shared_ptr<Device>& pDevice, uint32_t bufferCount /*= 1*/)
		: DeviceBoundObject(pDevice), m_BufferCount(bufferCount)
	{
		if (bufferCount == 0)
			throw InvalidArgumentError("The command buffer count should not be 0!");
	}
}