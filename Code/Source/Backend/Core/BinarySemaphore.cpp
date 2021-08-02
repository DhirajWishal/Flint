// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/BinarySemaphore.hpp"

#include <chrono>
#include <thread>

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
		using namespace std::literals;

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