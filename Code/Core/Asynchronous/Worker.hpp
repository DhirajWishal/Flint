// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <list>
#include <thread>
#include <future>
#include <functional>
#include <type_traits>
#include <condition_variable>

#include "Core/DataType.hpp"

namespace Flint
{
	namespace Async
	{
		/**
		 * Asynchronous worker thread class.
		 * This class contains a single worker thread and is used to submit commands to it.
		 */
		class Worker
		{
		public:
			/**
			 * Default constructor.
			 * This will construct and initiate the thread.
			 */
			Worker();

			/**
			 * Destructor.
			 */
			~Worker();

			/**
			 * Issue an asynchronous function to the worker to execute.
			 *
			 * @tparam Function The function type.
			 * @tparam Arguments The function arguments.
			 * @tparam ReturnType The return type of the function. This can either be specified explicitly or can be deduced implicitly.
			 * @param function The function to be executed asynchronously.
			 * @param arguments The arguments needed by the asynchronous function.
			 * @return A promise object of the function return.
			 */
			template<class Function, class... Arguments, class ReturnType = std::invoke_result_t<Function, Arguments...>>
			std::future<ReturnType> IssueAsync(Function&& function, Arguments&&... arguments)
			{
				// Lock the resources.
				auto uniqueLock = std::unique_lock(mMutex);

				// If the return type is void, we don't need to assign anything to a promise. 
				// Because of this, we can simply call the provided function and return an empty future.
				if constexpr (std::is_void_v<ReturnType>)
				{
					// Emplace the lambda to the list.
					mCommands.emplace_back([function, arguments...]() { function(std::forward<Arguments>(arguments)...); });

					// Notify the thread that we inserted a new command.
					mConditionVariable.notify_one();

					return std::future<ReturnType>();
				}
				else
				{
					// If the return type is not void, we need a promise because we must return its value.
					// But as for captures, we cannot move the promise itself, or as a unique pointer, so we use a shared pointer.
					std::shared_ptr<std::promise<ReturnType>> pPromise = std::make_shared<std::promise<ReturnType>>();

					// Get the future from the promise.
					auto future = pPromise->get_future();

					// Issue the command and notify the worker.
					mCommands.emplace_back([pPromise = std::move(pPromise), function, arguments...](){ pPromise->set_value(std::move(function(std::forward<Arguments>(arguments)...))); });
					mConditionVariable.notify_one();

					return future;
				}
			}

			/**
			 * Get the wait duration of the worker.
			 * 
			 * @return The duration in milliseconds.
			 */
			std::chrono::milliseconds GetWaitDuration() const { return mWaitDuration; }

			/**
			 * Set the worker thread's wait duration.
			 * 
			 * @param duration The wait duration to set.
			 */
			void SetWaitDuration(const std::chrono::milliseconds duration) { mWaitDuration = duration; }

			/**
			 * Get the number of asynchronous commands waiting in the queue.
			 * 
			 * @return The command count.
			 */
			uint64 GetAsyncCommandCount() const { return mCommands.size(); }

		private:
			/**
			 * The worker function
			 *
			 * @param commands The commands to be executed.
			 * @param mutex The resource lock.
			 * @param shouldRun Boolean stating if the thread should keep running.
			 * @param conditionVariable The condition variable used for synchronization.
			 * @param waitDuration The number of milliseconds to block the thread and wait.
			 */
			static void WorkerFunction(std::list<std::function<void()>>& commands, std::mutex& mutex, const bool& shouldRun, std::condition_variable& conditionVariable, const std::chrono::milliseconds& waitDuration);

		private:
			std::jthread mWorkerThread;
			std::list<std::function<void()>> mCommands;
			std::mutex mMutex;
			std::condition_variable mConditionVariable;
			std::chrono::milliseconds mWaitDuration;
			bool bShouldRun = true;
		};
	}
}