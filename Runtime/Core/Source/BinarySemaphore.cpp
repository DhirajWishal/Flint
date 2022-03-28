// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/BinarySemaphore.hpp"

namespace Flint
{
	BinarySemaphore::BinarySemaphore(const BinarySemaphore& other)
	{
		m_AtomicBool.store(other.m_AtomicBool);
	}

	void BinarySemaphore::Release()
	{
		m_AtomicBool = true;
	}

	void BinarySemaphore::Acquire()
	{
		while (m_AtomicBool != true);
		m_AtomicBool = false;
	}

	bool BinarySemaphore::TryAcquire()
	{
		if (m_AtomicBool != true)
			return false;

		m_AtomicBool = false;
		return true;
	}
}