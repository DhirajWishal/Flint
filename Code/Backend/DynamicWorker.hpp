// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBufferList.hpp"

#include <thread>
#include <atomic>

namespace Flint
{
	class WorkerCommandCenter
	{
	public:
		WorkerCommandCenter(const std::atomic<std::shared_ptr<CommandBufferList>>& pCommandBufferList) {}
	};

	class DynamicWorker
	{
	public:
		DynamicWorker() {}
	};

	std::thread CreateDynamicWorker(const std::atomic<std::shared_ptr<CommandBufferList>>& pCommandBufferList, const std::atomic<std::unique_ptr<WorkerCommandCenter>>& pCommandCenter);
}