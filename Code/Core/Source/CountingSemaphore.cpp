// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/CountingSemaphore.hpp"

namespace Flint
{
	void CountingSemaphore::Release()
	{
		mAtomicCounter++;
	}

	void CountingSemaphore::Acquire(boost::atomic<UI64> count)
	{
		while (mAtomicCounter != count);
		mAtomicCounter = 0;
	}

	bool CountingSemaphore::TryAcquire(boost::atomic<UI64> count)
	{
		if (mAtomicCounter != count)
			return false;

		mAtomicCounter = 0;
		return true;
	}
}