// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Display.hpp"

namespace Flint
{
	/**
	 * Next image info structure.
	 */
	struct NextImageInfo
	{
		NextImageInfo() = default;
		NextImageInfo(UI32 index, bool shouldRecreate) : mIndex(index), bShouldRecreate(shouldRecreate) {}

		UI32 mIndex = 0;
		bool bShouldRecreate = 0;
	};

	/**
	 * Flint swap chain object.
	 * Swap chains are used to render images to the display after being rendered to a render target.
	 */
	class SwapChain : public DeviceBoundObject
	{
	public:
		/**
		 * Construct the swap chain.
		 *
		 * @param pDevice: The device pointer.
		 * @param pDisplay: The display pointer.
		 * @param imageCount: The image count of the swap chain.
		 * @param presntMode: The swap chain present mode. If the requested present mode is not supported, an exception is thrown.
		 */
		SwapChain(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, UI32 imageCount, SwapChainPresentMode presentMode);

		/**
		 * Recreate the swap chain.
		 * This is necessary if the display is resized.
		 */
		virtual void Recreate() = 0;

		/**
		 * Acquire the next swap chain image.
		 * 
		 * @param frameIndex: The frame index of the swap chain.
		 * @return The next image info structure.
		 */
		virtual NextImageInfo AcquireNextImage(UI32 frameIndex) = 0;

	public:
		/**
		 * Get the display pointer.
		 *
		 * @return The display pointer.
		 */
		const std::shared_ptr<Display> GetDisplay() const { return pDisplay; }

		/**
		 * Get the extent of the swap chain.
		 *
		 * @return The extent.
		 */
		const FBox2D GetExtent() const { return mExtent; }

		/**
		 * Get the image count of the swap chain.
		 *
		 * @return The image count.
		 */
		const UI32 GetImageCount() const { return mImageCount; }

		/**
		 * Get the pixel format of the swap chain.
		 *
		 * @return The pixel format.
		 */
		const PixelFormat GetPixelFormat() const { return mPixelForamt; }

		/**
		 * Get the swap chain present mode.
		 * 
		 * @return The present mode.
		 */
		const SwapChainPresentMode GetPresentMode() const { return mPresentMode; }

	protected:
		std::shared_ptr<Display> pDisplay = nullptr;

		FBox2D mExtent = {};
		UI32 mImageCount = 0;

		PixelFormat mPixelForamt = PixelFormat::UNDEFINED;
		SwapChainPresentMode mPresentMode = SwapChainPresentMode::MAILBOX;
	};
}