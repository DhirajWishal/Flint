// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>

namespace Flint
{
	/**
	 * Frame timer class.
	 * This keeps track of the time taken by one frame to the next frame.
	 */
	class FrameTimer final
	{
		using Clock = std::chrono::high_resolution_clock;
		using TimePoint = Clock::time_point;
		using Duration = Clock::duration;

	public:
		/**
		 * Default constructor.
		 */
		FrameTimer() : m_PreviousFrame(Clock::now()) {}

		/**
		 * Get the time difference in the clock's duration.
		 *
		 * @return The duration.
		 */
		[[nodiscard]] Duration tick();

		/**
		 * Convert a duration to frames per second.
		 *
		 * @param duration The duration of the frame.
		 * @return The frames per second.
		 */
		[[nodiscard]] static uint32_t FramesPerSecond(const Duration duration);

	private:
		TimePoint m_PreviousFrame;
	};
}