// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Graphical.hpp"
#include "Core/Errors/InvalidArgumentError.hpp"

namespace Flint
{
	namespace Backend
	{
		Graphical::Graphical(uint32_t frameCount)
			: m_FrameCount(frameCount), m_NeedToUpdate(frameCount)
		{
			if (frameCount == 0)
				throw InvalidArgumentError("The frame count should be grater than 0!");
		}
	}
}