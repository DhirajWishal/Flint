// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Asynchronous/Worker.hpp"

namespace Flint
{
	namespace Async
	{
		using namespace std::chrono_literals;

		Worker::Worker()
		{
			// Setup the thread.
			mWorkerThread = std::jthread([this]() { WorkerFunction(mCommands, mMutex, bShouldRun, mConditionVariable, mWaitDuration); });
			mWaitDuration = 500ms;
		}

		Worker::~Worker()
		{
			// Stop the thread from looping and wait till it finishes.
			bShouldRun = false;
			mWorkerThread.join();
		}

		void Worker::WorkerFunction(std::list<std::function<void()>>& commands, std::mutex& mutex, const bool& shouldRun, std::condition_variable& conditionVariable, const std::chrono::milliseconds& waitDuration)
		{
			while (shouldRun || commands.size())
			{
				auto uniqueLock = std::unique_lock(mutex);
				if (conditionVariable.wait_for(uniqueLock, waitDuration, [commands]() { return commands.empty() == false; }))
				{
					auto function = std::move(commands.back());
					commands.pop_back();

					function();
				}
			}
		}
	}
}