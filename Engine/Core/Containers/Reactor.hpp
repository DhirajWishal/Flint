// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <thread>
#include <functional>

namespace Flint
{
	/**
	 * Reactor class.
	 */
	class Reactor final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param thread The thread to run.
		 */
		explicit Reactor(std::jthread&& thread) : m_WorkerThread(thread) {}

	private:
		std::jthread m_WorkerThread;
	};
}