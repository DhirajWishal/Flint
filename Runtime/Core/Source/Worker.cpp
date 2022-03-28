// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Worker.hpp"

namespace Flint
{
	using namespace std::chrono_literals;

	Worker::Worker()
	{
		// Setup the thread.
		m_WorkerThread = std::jthread([this]() { WorkerFunction(m_Commands, m_Mutex, m_ConditionVariable, m_WaitDuration, bShouldRun); });
		m_WaitDuration = 500ms;
	}

	Worker::Worker(std::chrono::milliseconds duration) : m_WaitDuration(duration)
	{
		// Setup the thread.
		m_WorkerThread = std::jthread([this]() { WorkerFunction(m_Commands, m_Mutex, m_ConditionVariable, m_WaitDuration, bShouldRun); });
	}

	Worker::Worker(const Worker& other) : m_Commands(other.m_Commands), m_WaitDuration(other.m_WaitDuration)
	{
		// Setup the thread.
		m_WorkerThread = std::jthread([this]() { WorkerFunction(m_Commands, m_Mutex, m_ConditionVariable, m_WaitDuration, bShouldRun); });
	}

	Worker::Worker(Worker&& other) : m_Commands(std::move(other.m_Commands)), m_WaitDuration(std::move(other.m_WaitDuration))
	{
		other.bShouldRun = false;
		other.m_WorkerThread.join();

		// Setup the thread.
		m_WorkerThread = std::jthread([this]() { WorkerFunction(m_Commands, m_Mutex, m_ConditionVariable, m_WaitDuration, bShouldRun); });
	}

	Worker::~Worker()
	{
		// Stop the thread from looping and wait till it finishes.
		bShouldRun = false;
		m_WorkerThread.join();
	}

	Worker& Worker::operator=(const Worker& other)
	{
		m_Commands = other.m_Commands;
		m_WaitDuration = other.m_WaitDuration;
		return *this;
	}

	Worker& Worker::operator=(Worker&& other)
	{
		m_Commands = std::move(other.m_Commands);
		m_WaitDuration = std::move(other.m_WaitDuration);

		other.bShouldRun = false;
		other.m_WorkerThread.join();

		return *this;
	}

	void Worker::WorkerFunction(std::list<std::function<void()>>& commands, std::mutex& mutex, std::condition_variable& conditionVariable, const std::chrono::milliseconds& waitingDuration, const bool& shouldRun)
	{
		while (shouldRun || commands.size())
		{
			// Lock the resources.
			auto uniqueLock = std::unique_lock(mutex);

			// Check if we can execute a command. If not, we wait for a specified amount of time. If a command was submitted within that time, it is executed or else it will wait.
			if (!commands.empty() || conditionVariable.wait_for(uniqueLock, waitingDuration, [commands]() { return !commands.empty(); }))
			{
				auto function = std::move(commands.back());
				commands.pop_back();

				// Execute the work.
				function();
			}
		}
	}
}