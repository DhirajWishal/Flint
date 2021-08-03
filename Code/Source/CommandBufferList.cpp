// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "CommandBufferList.hpp"

namespace Flint
{
	CommandBufferList::CommandBufferList(const std::shared_ptr<Device>& pDevice, const UI32 bufferCount) : DeviceBoundObject(pDevice), mBufferCount(bufferCount)
	{
		if (bufferCount == 0)
			FLINT_THROW_INVALID_ARGUMENT("Command buffer count should be greater than 0!");
	}
}