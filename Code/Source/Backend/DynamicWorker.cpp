// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DynamicWorker.hpp"

namespace Flint
{
	namespace _Async
	{
		void WorkerFunction(const std::atomic<std::shared_ptr<CommandBufferList>>& pCommandBufferList, const std::atomic<std::unique_ptr<WorkerCommandCenter>>& pCommandCenter)
		{
			//DynamicWorker dynamicWorker{ pCommandBufferList };
		}
	}

	std::thread CreateDynamicWorker(const std::atomic<std::shared_ptr<CommandBufferList>>& pCommandBufferList, const std::atomic<std::unique_ptr<WorkerCommandCenter>>& pCommandCenter)
	{
		//return std::thread(_Async::WorkerFunction, pCommandBufferList, pCommandCenter);
		return std::thread();
	}
}