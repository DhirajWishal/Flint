// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

export module Flint.Core.BinarySemaphore;

import <atomic>;

export namespace Flint
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
		 * @param other The other semaphore.
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
		std::atomic<bool> mAtomicBool = false;
	};
}

module: private;

namespace Flint
{
	BinarySemaphore::BinarySemaphore(const BinarySemaphore& other)
	{
		mAtomicBool.store(other.mAtomicBool);
	}

	void BinarySemaphore::Release()
	{
		mAtomicBool = true;
	}

	void BinarySemaphore::Acquire()
	{
		while (mAtomicBool != true);
		mAtomicBool = false;
	}

	bool BinarySemaphore::TryAcquire()
	{
		if (mAtomicBool != true)
			return false;

		mAtomicBool = false;
		return true;
	}
}