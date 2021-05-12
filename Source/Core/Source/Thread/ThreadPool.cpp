// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Thread/ThreadPool.h"

namespace Flint
{
	namespace Thread
	{
		void ThreadPool::WaitForThread(UI8 index)
		{
			mThreads[index].join();
		}

		void ThreadPool::Wait()
		{
			for (auto itr = mThreads.begin(); itr != mThreads.end(); itr++)
				itr->join();
		}

		bool ThreadPool::IsExecuting(UI8 index) const
		{
			return !mThreads[index].joinable();
		}
	}
}