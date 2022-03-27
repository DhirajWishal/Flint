// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Worker.hpp"

namespace Flint
{
	/**
	 * Work group class.
	 * This class contains a bunch of workers, where the work is divided among them in a deterministic manner.
	 */
	class WorkGroup
	{
	public:
		/**
		 * Constructor.
		 * Construct the work group using the worker count.
		 *
		 * @param groupSize The number of workers in the work group.
		 */
		explicit WorkGroup(uint64_t groupSize) : mWorkers(groupSize) {}

		/**
		 * Constructor.
		 * Construct the work group using the worker count and their duration.
		 *
		 * @param groupSize The number of workers in the work group.
		 * @param duration The duration to wait for all the workers.
		 */
		explicit WorkGroup(uint64_t groupSize, std::chrono::milliseconds duration) : mWorkers(groupSize, Worker(duration)) {}

		/**
		 * Issue work to a worker to execute.
		 * Work is assigned in a deterministic way (index % groupSize).
		 *
		 * @tparam Function The function type.
		 * @tparam Arguments The function arguments.
		 * @tparam ReturnType The return type of the function. This can either be specified explicitly or can be deduced implicitly.
		 * @param function The function to be executed on another thread.
		 * @param arguments The arguments needed by the work function.
		 * @return A promise object of the function return.
		 */
		template<class Function, class... Arguments, class ReturnType = std::invoke_result_t<Function, Arguments...>>
		std::future<ReturnType> IssueWork(Function&& function, Arguments&&... arguments) { return mWorkers[GetNextIndex(mIndex++)].IssueWork(std::forward<Function>(function), std::forward<Arguments>(arguments)...); }

		/**
		 * Get the work group size.
		 *
		 * @return The work group size.
		 */
		uint64_t GetSize() const { return mWorkers.size(); }

		/**
		 * Resize the work group.
		 *
		 * @param size The required size.
		 */
		void Resize(uint64_t size) { mWorkers.resize(size); }

		/**
		 * Get the current index.
		 * 
		 * @return The index.
		 */
		uint64_t GetCurrentIndex() const { return mIndex; }

		/**
		 * Get a worker from the group.
		 * 
		 * @param index The index of the worker.
		 * @return The worker reference.
		 */
		Worker& GetWorker(uint64_t index) { return mWorkers[index]; }

	private:
		/**
		 * Get the index of the next position to assign the work.
		 *
		 * @param index The previous index.
		 * @return The index.
		 */
		uint64_t GetNextIndex(uint64_t index) const { return index % mWorkers.size(); }

	private:
		std::vector<Worker> mWorkers = {};
		uint64_t mIndex = 0;
	};
}