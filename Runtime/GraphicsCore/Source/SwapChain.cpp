// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/SwapChain.hpp"

namespace Flint
{
	SwapChain::SwapChain(Device* pDevice, Display* pDisplay, uint32_t imageCount, const SwapChainPresentMode presentMode)
		: DeviceBoundObject(pDevice), pDisplay(pDisplay), mExtent(pDisplay->GetExtent()), mImageCount(imageCount), mPresentMode(presentMode)
	{
		if (!pDisplay)
			throw std::invalid_argument("Display pointer should not be null!");

		if (imageCount == 0)
			throw std::invalid_argument("Swap chain image count should not be zero!");
	}
}