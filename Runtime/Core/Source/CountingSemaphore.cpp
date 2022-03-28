// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/CountingSemaphore.hpp"

namespace Flint
{
	void CountingSemaphore::Release()
	{
		m_AtomicCounter++;
	}

	void CountingSemaphore::Acquire(std::atomic<uint64_t> count)
	{
		while (m_AtomicCounter != count);
		m_AtomicCounter = 0;
	}

	bool CountingSemaphore::TryAcquire(std::atomic<uint64_t> count)
	{
		if (m_AtomicCounter != count)
			return false;

		m_AtomicCounter = 0;
		return true;
	}
}