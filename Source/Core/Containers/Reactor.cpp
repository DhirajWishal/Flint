// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Core/Containers/Reactor.hpp"

#include <Optick.h>

namespace Flint
{
	Reactor::Reactor()
		: m_Worker(&Reactor::worker, this)
	{
	}

	Reactor::~Reactor()
	{
		// Set the should stop boolean to true, to indicate that we're about to finish executing.
		{
			auto lock = std::scoped_lock(m_Mutex);
			m_bShouldRun = true;
		}

		m_Conditional.notify_one();

		// Now wait till the thread 
		m_Worker.join();
	}

	void Reactor::issueCommand(std::function<void()>&& function)
	{
		// Issue the function to the queue.
		{
			auto lock = std::scoped_lock(m_Mutex);
			m_Functions.push(std::move(function));
		}

		// Notify that we have a command ready.
		m_Conditional.notify_one();
	}

	void Reactor::worker()
	{
		OPTICK_THREAD("Reactor Worker Thread");
		auto lock = std::unique_lock(m_Mutex);

		do
		{
			OPTICK_EVENT();

			// Wait until we have something to execute, or if we need to stop execution.
			m_Conditional.wait(lock, [this]
				{
					return !m_Functions.empty() || m_bShouldRun;
				}
			);

			// If we have something to execute, lets execute one function.
			if (!m_Functions.empty())
			{
				auto function = std::move(m_Functions.front());
				m_Functions.pop();

				// Unlock resources now that we can execute the function.
				lock.unlock();

				// Execute the function.
				function();

				// Lock everything back in place.
				lock.lock();
			}

		} while (m_bShouldRun);

		// Unlock resources now, we can finish everything.
		lock.unlock();

		// Lets finish all the functions if we have any so we don't miss something important.
		while (!m_Functions.empty())
		{
			// Get the function and remove it from the list.
			auto function = std::move(m_Functions.front());
			m_Functions.pop();

			// Execute the function.
			function();
		}
	}
}