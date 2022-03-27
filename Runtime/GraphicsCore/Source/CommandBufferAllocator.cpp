// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/CommandBufferAllocator.hpp"

namespace Flint
{
	CommandBufferAllocator::CommandBufferAllocator(Device* pDevice, const uint32_t bufferCount)
		: DeviceBoundObject(pDevice), mBufferCount(bufferCount)
	{
		if (bufferCount == 0)
			throw std::invalid_argument("Command buffer count should not be 0!");
	}

	CommandBufferAllocator::CommandBufferAllocator(Device* pDevice, const std::shared_ptr<CommandBufferAllocator>& pParent, const uint32_t bufferCount)
		: DeviceBoundObject(pDevice), pParentAllocator(pParent), mBufferCount(bufferCount)
	{
		if (!pParentAllocator)
			throw std::invalid_argument("The parent command buffer allocator pointer should not be null!");

		if (bufferCount == 0)
			throw std::invalid_argument("Command buffer count should not be 0!");
	}
}