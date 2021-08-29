// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <boost/atomic.hpp>

namespace Flint
{
	/**
	 * Flint binary semaphore object.
	 * This object is thread safe and can be used to synchronize two threads.
	 */
	class BinarySemaphore
	{
	public:
		BinarySemaphore() = default;

		/**
		 * Copy constructor.
		 *
		 * @param other: The other semaphore.
		 */
		BinarySemaphore(const BinarySemaphore& other);

		/**
		 * Release the semaphore.
		 */
		void Release();

		/**
		 * Acquire the semaphore.
		 * This waits until the other thread releases it.
		 */
		void Acquire();

		/**
		 * Try and acquire the semaphore.
		 * This does not wait until the other thread releases it.
		 *
		 * @return Whether or not if its acquired.
		 */
		bool TryAcquire();

	private:
		boost::atomic<bool> mAtomicBool = false;
	};
}