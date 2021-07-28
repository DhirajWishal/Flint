// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Display.hpp"

namespace Flint
{
	Display::Display(const std::shared_ptr<Instance>& pInstance, const FExtent2D& extent, const std::string& title) : pInstance(pInstance), mExtent(extent), mTitle(title)
	{
		if (!pInstance)
			FLINT_THROW_INVALID_ARGUMENT("Instance pointer should not be null!");

		if (extent.mWidth == 0 || extent.mHeight == 0)
			FLINT_THROW_INVALID_ARGUMENT("Display width and height should be greater than 0!");
	}
}