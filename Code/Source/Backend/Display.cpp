// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Display.hpp"

namespace Flint
{
	Display::Display(const std::shared_ptr<Instance>& pInstance, const FBox2D& extent, const std::string& title) : pInstance(pInstance), mExtent(extent), mTitle(title)
	{
		if (!pInstance)
			FLINT_THROW_INVALID_ARGUMENT("Instance pointer should not be null!");
	}

	const std::vector<std::filesystem::path> Display::GetDragAndDropValues()
	{
		return std::move(mDragAndDropPaths);
	}
}