// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <atomic>
#include <thread>

namespace Flint
{
	/**
	 * Spin mutex class.
	 * Instead of blocking the thread, this will use a while loop to check if the lock was released by the owning thread. This is good for performance if the mutex is not locked for long amounts of time.
	 * If a mutex is required which will be locked for longer periods of time, use something else like std::mutex or std::recursive_mutex.
	 *
	 * Just like normal mutexes, you can use this with std::lock_guard, std::unique_lock and std::scoped_lock.
	 */
	class SpinMutex
	{
		using ValueType = std::thread::id;

	public:
		/**
		 * Default constructor.
		 */
		SpinMutex() = default;

		/**
		 * Check if the spin lock is locked.
		 *
		 * @return Whether or not it's locked.
		 */
		[[nodiscard]] bool is_locked() const { return m_State != std::this_thread::get_id() && m_State != ValueType(); }

		/**
		 * Wait if it's locked.
		 */
		void wait() const { while (is_locked()); }

		/**
		 * Lock the spin lock.
		 * If this is locked by another thread, it will wait till the lock is unlocked.
		 */
		void lock() { wait(); m_State = std::this_thread::get_id(); }

		/**
		 * Unlock the lock.
		 */
		void unlock() { m_State = ValueType(); }

		/**
		 * Try and lock.
		 * This will return true if we were able to lock, and return false if failed.
		 *
		 * @return The state.
		 */
		[[nodiscard]] bool try_lock() { return is_locked() ? false : (m_State = std::this_thread::get_id(), true); }

	private:
		std::atomic<ValueType> m_State;
	};
}