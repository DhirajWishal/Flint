// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Core/DataTypes.h"

#include <chrono>

namespace Flint
{
	namespace Thread
	{
		namespace Utilities
		{
			/**
			 * Get the number of usable thread count.
			 */
			UI8 GetUsableThreadCount();

			/**
			 * Sleep this thread for a period of time.
			 *
			 * @param duration: The duration in nanoseconds.
			 */
			void SleepThisThread(std::chrono::nanoseconds duration);
		}
	}
}