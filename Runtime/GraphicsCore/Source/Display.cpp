// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/Display.hpp"

namespace Flint
{
	Display::Display(Instance* pInstance, const FBox2D& extent, const std::string& title) : pInstance(pInstance), mExtent(extent), mTitle(title)
	{
		if (!pInstance)
			throw std::invalid_argument("Instance pointer should not be null!");
	}

	const std::vector<std::filesystem::path> Display::GetDragAndDropValues()
	{
		return std::move(mDragAndDropPaths);
	}
}