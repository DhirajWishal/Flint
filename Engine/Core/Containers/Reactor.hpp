// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <functional>
#include <condition_variable>

namespace Flint
{
	/**
	 * Reactor class.
	 */
	class Reactor final
	{
	public:
		/**
		 * Default constructor.
		 */
		Reactor();

		/**
		 * Default destructor.
		 */
		~Reactor();

		/**
		 * Issue a new command to the reactor.
		 *
		 * @param function The function to run on the other thread.
		 */
		void issueCommand(std::function<void()>&& function);

	private:
		/**
		 * Worker function.
		 * This function is run on the worker thread.
		 */
		void worker();

	private:
		std::jthread m_Worker;
		std::queue<std::function<void()>> m_Functions;
		std::condition_variable m_Conditional;
		std::mutex m_Mutex;

		bool m_bShouldRun = true;
	};
}