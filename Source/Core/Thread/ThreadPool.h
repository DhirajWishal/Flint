// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"

#include <thread>

namespace Flint
{
	namespace Thread
	{
		/**
		 * Thread pool object.
		 * This object holds and controls multiple threads.
		 */
		class ThreadPool {
		public:
			ThreadPool() {}
			~ThreadPool() {}

			void SetThreadCount(UI8 count) { mThreadCount = count; mThreads.resize(mThreadCount); }
			UI8 GetThreadCount() const { return mThreadCount; }

			/**
			 * Issue a thread function to the pool.
			 *
			 * @tparam ThreadFunc: The thread function type.
			 * @tparam The function's VA args type.
			 * @param index: The index of the thread pool.
			 * @param func: The function to be passed.
			 * @param args: the arguments to be passed to the function.
			 */
			template<class ThreadFunc, class ...Args>
			void IssueThreadFunction(UI8 index, ThreadFunc func, Args&... args)
			{
				mThreads[index].swap(std::thread(func, args...));
			}

			/**
			 * Wait till a single thread finishes execution.
			 *
			 * @param index: The index of the thread.
			 */
			void WaitForThread(UI8 index);

			/**
			 * Wait till all the threads finish execution.
			 */
			void Wait();

			/**
			 * Check if a thread has finished its execution.
			 *
			 * @param index: The index of the thread to be checked.
			 */
			bool IsExecuting(UI8 index) const;

		private:
			std::vector<std::thread> mThreads;
			UI8 mThreadCount = 0;
		};
	}
}