// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/CountingSemaphore.hpp"

namespace Flint
{
	void CountingSemaphore::Release()
	{
		mAtomicCounter++;
	}

	void CountingSemaphore::Acquire(std::atomic<uint64_t> count)
	{
		while (mAtomicCounter != count);
		mAtomicCounter = 0;
	}

	bool CountingSemaphore::TryAcquire(std::atomic<uint64_t> count)
	{
		if (mAtomicCounter != count)
			return false;

		mAtomicCounter = 0;
		return true;
	}
}