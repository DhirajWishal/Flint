// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/SwapChain.hpp"

namespace Flint
{
	SwapChain::SwapChain(const boost::shared_ptr<Device>& pDevice, const boost::shared_ptr<Display>& pDisplay, UI32 imageCount, const SwapChainPresentMode presentMode)
		: DeviceBoundObject(pDevice), pDisplay(pDisplay), mExtent(pDisplay->GetExtent()), mImageCount(imageCount), mPresentMode(presentMode)
	{
		if (!pDisplay)
			FLINT_THROW_INVALID_ARGUMENT("Display pointer should not be null!");

		if (imageCount == 0)
			FLINT_THROW_INVALID_ARGUMENT("Swap chain image count should not be zero!");
	}
}