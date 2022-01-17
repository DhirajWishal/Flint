// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Worker.hpp"

namespace Flint
{
	using namespace std::chrono_literals;

	Worker::Worker()
	{
		// Setup the thread.
		mWorkerThread = std::jthread([this]()
			{
				while (bShouldRun || mCommands.size())
				{
					// Lock the resources.
					auto uniqueLock = std::unique_lock(mMutex);

					// Check if we can execute a command. If not, we wait for a specified amount of time. If a command was submitted within that time, it is executed or else it will wait.
					if (!mCommands.empty() || mConditionVariable.wait_for(uniqueLock, mWaitDuration, [this]() { return !mCommands.empty(); }))
					{
						auto function = std::move(mCommands.back());
						mCommands.pop_back();

						// Execute the work.
						function();
					}
				}
			});

		mWaitDuration = 500ms;
	}

	Worker::Worker(std::chrono::milliseconds duration) : mWaitDuration(duration)
	{
		// Setup the thread.
		mWorkerThread = std::jthread([this]()
			{
				while (bShouldRun || mCommands.size())
				{
					// Lock the resources.
					auto uniqueLock = std::unique_lock(mMutex);

					// Check if we can execute a command. If not, we wait for a specified amount of time. If a command was submitted within that time, it is executed or else it will wait.
					if (!mCommands.empty() || mConditionVariable.wait_for(uniqueLock, mWaitDuration, [this]() { return !mCommands.empty(); }))
					{
						auto function = std::move(mCommands.back());
						mCommands.pop_back();

						// Execute the work.
						function();
					}
				}
			});
	}

	Worker::Worker(const Worker& other) : mCommands(other.mCommands), mWaitDuration(other.mWaitDuration)
	{
		// Setup the thread.
		mWorkerThread = std::jthread([this]()
			{
				while (bShouldRun || mCommands.size())
				{
					// Lock the resources.
					auto uniqueLock = std::unique_lock(mMutex);

					// Check if we can execute a command. If not, we wait for a specified amount of time. If a command was submitted within that time, it is executed or else it will wait.
					if (!mCommands.empty() || mConditionVariable.wait_for(uniqueLock, mWaitDuration, [this]() { return !mCommands.empty(); }))
					{
						auto function = std::move(mCommands.back());
						mCommands.pop_back();

						// Execute the work.
						function();
					}
				}
			});
	}

	Worker::Worker(Worker&& other) : mCommands(std::move(other.mCommands)), mWaitDuration(std::move(other.mWaitDuration))
	{
		other.bShouldRun = false;
		other.mWorkerThread.join();

		// Setup the thread.
		mWorkerThread = std::jthread([this]()
			{
				while (bShouldRun || mCommands.size())
				{
					// Lock the resources.
					auto uniqueLock = std::unique_lock(mMutex);

					// Check if we can execute a command. If not, we wait for a specified amount of time. If a command was submitted within that time, it is executed or else it will wait.
					if (!mCommands.empty() || mConditionVariable.wait_for(uniqueLock, mWaitDuration, [this]() { return !mCommands.empty(); }))
					{
						auto function = std::move(mCommands.back());
						mCommands.pop_back();

						// Execute the work.
						function();
					}
				}
			});
	}

	Worker::~Worker()
	{
		// Stop the thread from looping and wait till it finishes.
		bShouldRun = false;
		mWorkerThread.join();
	}

	Worker& Worker::operator=(const Worker& other)
	{
		mCommands = other.mCommands;
		mWaitDuration = other.mWaitDuration;
		return *this;
	}

	Worker& Worker::operator=(Worker&& other)
	{
		mCommands = std::move(other.mCommands);
		mWaitDuration = std::move(other.mWaitDuration);

		other.bShouldRun = false;
		other.mWorkerThread.join();

		return *this;
	}
}