// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/SwapChain.hpp"

namespace Flint
{
	SwapChain::SwapChain(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, uint32 imageCount, const SwapChainPresentMode presentMode)
		: DeviceBoundObject(pDevice), pDisplay(pDisplay), mExtent(pDisplay->GetExtent()), mImageCount(imageCount), mPresentMode(presentMode)
	{
		if (!pDisplay)
			throw std::invalid_argument("Display pointer should not be null!");

		if (imageCount == 0)
			throw std::invalid_argument("Swap chain image count should not be zero!");
	}
}