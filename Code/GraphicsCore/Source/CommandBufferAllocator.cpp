// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/CommandBufferAllocator.hpp"

namespace Flint
{
	CommandBufferAllocator::CommandBufferAllocator(const boost::shared_ptr<Device>& pDevice, const UI32 bufferCount)
		: DeviceBoundObject(pDevice), mBufferCount(bufferCount)
	{
		if (bufferCount == 0)
			FLINT_THROW_INVALID_ARGUMENT("Command buffer count should not be 0!");
	}

	CommandBufferAllocator::CommandBufferAllocator(const boost::shared_ptr<Device>& pDevice, const boost::shared_ptr<CommandBufferAllocator>& pParent, const UI32 bufferCount)
		: DeviceBoundObject(pDevice), pParentAllocator(pParent), mBufferCount(bufferCount)
	{
		if (!pParentAllocator)
			FLINT_THROW_INVALID_ARGUMENT("The parent command buffer allocator pointer should not be null!");

		if (bufferCount == 0)
			FLINT_THROW_INVALID_ARGUMENT("Command buffer count should not be 0!");
	}
}