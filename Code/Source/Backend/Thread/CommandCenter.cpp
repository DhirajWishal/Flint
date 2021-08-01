// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Thread/CommandCenter.hpp"
#include "Core/Profiler.hpp"

namespace Flint
{
	namespace Thread
	{
		bool CommandCenter::AddCommand(std::unique_ptr<CommandBase>&& pCommand)
		{
			FLINT_SETUP_PROFILER();

			if (pCommandQueue.size() == mMaxCommandQueueLength)
				return false;

			pCommandQueue.push(std::move(pCommand));
			return true;
		}

		std::unique_ptr<CommandBase> CommandCenter::GetCommand()
		{
			FLINT_SETUP_PROFILER();

			if (pCommandQueue.empty())
				return nullptr;

			std::unique_ptr<CommandBase> pCommand = std::move(pCommandQueue.front());
			pCommandQueue.pop();

			return std::move(pCommand);
		}
	}
}