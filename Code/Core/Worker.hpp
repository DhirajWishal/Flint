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
	/**
	 * Worker thread class.
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
		 * Constructor.
		 * Set the duration for the worker to wait.
		 * 
		 * @param duration The waiting duration.
		 */
		explicit Worker(std::chrono::milliseconds duration);

		/**
		 * Copy constructor.
		 *
		 * @param other The other worker.
		 */
		Worker(const Worker& other);

		/**
		 * Move constructor.
		 *
		 * @param other The other worker.
		 */
		Worker(Worker&& other);

		/**
		 * Destructor.
		 */
		~Worker();

		/**
		 * Issue work to the worker to execute.
		 *
		 * @tparam Function The function type.
		 * @tparam Arguments The function arguments.
		 * @tparam ReturnType The return type of the function. This can either be specified explicitly or can be deduced implicitly.
		 * @param function The function to be executed on another thread.
		 * @param arguments The arguments needed by the work function.
		 * @return A promise object of the function return.
		 */
		template<class Function, class... Arguments, class ReturnType = std::invoke_result_t<Function, Arguments...>>
		std::future<ReturnType> IssueWork(Function&& function, Arguments&&... arguments)
		{
			// Lock the resources.
			auto uniqueLock = std::unique_lock(mMutex);
			auto future = std::future<ReturnType>();

			// If the return type is void, we don't need to assign anything to a promise. 
			// Because of this, we can simply call the provided function and return an empty future.
			if constexpr (std::is_void_v<ReturnType>)
			{
				// Emplace the lambda to the list.
				mCommands.emplace_back(
					[function, arguments...]()
				{
					function(arguments...);
				});
			}
			else
			{
				// If the return type is not void, we need a promise because we must return its value.
				// But as for captures, we cannot move the promise itself, so we use a pointer for that purpose.
				// Even though a smart pointer will work, this is somewhat of an overkill as we can easily manage the pointer and cut down the overhead of using a shared pointer.
				const auto pPromise = new std::promise<ReturnType>();

				// Get the future from the promise.
				future = pPromise->get_future();

				// Issue the command.
				mCommands.emplace_back(
					[pPromise, function, arguments...]()
				{
					pPromise->set_value(std::move(function(arguments...)));
					delete pPromise;
				});
			}

			// Notify the thread that we inserted a new command.
			mConditionVariable.notify_one();

			return future;
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
		 * Get the number of work commands waiting in the queue.
		 *
		 * @return The command count.
		 */
		uint64 GetAsyncCommandCount() const { return mCommands.size(); }

	public:
		/**
		 * Copy assign operator.
		 * 
		 * @param other The other worker.
		 * @return This object reference.
		 */
		Worker& operator=(const Worker& other);

		/**
		 * Move assign operator.
		 *
		 * @param other The other worker.
		 * @return This object reference.
		 */
		Worker& operator=(Worker&& other);

	private:
		std::jthread mWorkerThread = {};
		std::list<std::function<void()>> mCommands = {};
		std::mutex mMutex = {};
		std::condition_variable mConditionVariable = {};
		std::chrono::milliseconds mWaitDuration = {};
		bool bShouldRun = true;
	};
}