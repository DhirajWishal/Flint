// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Core/DataTypes.h"

namespace Flint
{
	namespace Benchmark
	{
		/**
		 * Timer object for Flint Benchmark.
		 * This object calculates and prints the time taken to execute some code within a specific scope.
		 */
		class Timer {
		public:
			/**
			 * Begin the timer without an output text.
			 */
			Timer();

			/**
			 * Begin the timer with an output text.
			 */
			Timer(const wchar* pText);

			/**
			 * Destroy the timer.
			 * This prints the total time taken for the code execution.
			 */
			~Timer();

		private:
			I64 mStartTime = 0;	// The start time point.
			const wchar* pTimerName = nullptr;	// The timer name.
		};
	}
}