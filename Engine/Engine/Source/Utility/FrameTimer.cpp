// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/Utility/FrameTimer.hpp"

namespace Flint
{
	FrameTimer::Duration FrameTimer::tick()
	{
		const auto current = Clock::now();
		const auto diff = current - m_PreviousFrame;
		m_PreviousFrame = current;

		return diff;
	}
}