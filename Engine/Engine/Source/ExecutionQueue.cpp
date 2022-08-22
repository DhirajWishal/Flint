// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/ExecutionQueue.hpp"

namespace Flint
{
	ExecutionQueue::ExecutionQueue(const std::shared_ptr<Device>& pDevice)
		: DeviceBoundObject(pDevice), m_pCommandBuffers(pDevice->createCommandBuffers(1))
	{
	}

	void ExecutionQueue::execute()
	{

	}
}