// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "SwapChain.hpp"

namespace Flint
{
	SwapChain::SwapChain(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, UI32 imageCount, SwapChainPresentMode presentMode)
		: DeviceBoundObject(pDevice), pDisplay(pDisplay), mExtent(pDisplay->GetExtent()), mImageCount(imageCount), mPresentMode(presentMode)
	{
		if (!pDisplay)
			FLINT_THROW_INVALID_ARGUMENT("Display pointer should not be null!");

		if (imageCount == 0)
			FLINT_THROW_INVALID_ARGUMENT("Swap chain image count should not be zero!");
	}
}