// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Thread/ThreadUtilities.h"

#include <thread>

namespace Flint
{
	namespace Thread
	{
		namespace Utilities
		{
			UI8 GetUsableThreadCount()
			{
				return std::thread::hardware_concurrency();
			}

			void SleepThisThread(std::chrono::nanoseconds duration)
			{
				std::this_thread::sleep_for(duration);
			}
		}
	}
}