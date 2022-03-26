// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DataType.hpp"
#include "BinarySemaphore.hpp"

namespace Flint
{
	/**
	 * Flint counting semaphore object.
	 * This object can be used with multiple threads and is used for synchronization.
	 */
	class CountingSemaphore
	{
	public:
		CountingSemaphore() = default;

		/**
		 * Release the counting semaphore.
		 * This will increment the atomic counter by 1.
		 */
		void Release();

		/**
		 * Acquire the semaphore.
		 * This will wait until the atomic counter reaches the count value.
		 *
		 * @param count The count to wait until.
		 */
		void Acquire(std::atomic<uint64> count);

		/**
		 * Try and acquire the semaphore.
		 * This works the same as Acquire, but will return false if could not acquire.
		 *
		 * @param count The count to wait for.
		 * @return Whether or not it acquired.
		 */
		bool TryAcquire(std::atomic<uint64> count);

	private:
		std::atomic<uint64> mAtomicCounter = 0;
	};
}