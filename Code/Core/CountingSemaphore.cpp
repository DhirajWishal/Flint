// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

export module Flint.Core.CountingSemaphore;

import Flint.Core.DataType;
import Flint.Core.BinarySemaphore;

import <atomic>;

export namespace Flint
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

module: private;

namespace Flint
{
	void CountingSemaphore::Release()
	{
		mAtomicCounter++;
	}

	void CountingSemaphore::Acquire(std::atomic<uint64> count)
	{
		while (mAtomicCounter != count);
		mAtomicCounter = 0;
	}

	bool CountingSemaphore::TryAcquire(std::atomic<uint64> count)
	{
		if (mAtomicCounter != count)
			return false;

		mAtomicCounter = 0;
		return true;
	}
}
