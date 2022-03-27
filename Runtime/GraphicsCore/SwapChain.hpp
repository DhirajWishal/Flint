// Copyright 2021-2022 Dhiraj Wishal
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
		NextImageInfo(uint32_t index, bool shouldRecreate) : mIndex(index), bShouldRecreate(shouldRecreate) {}

		uint32_t mIndex = 0;
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
		 * @param pDevice The device pointer.
		 * @param pDisplay The display pointer.
		 * @param imageCount The image count of the swap chain.
		 * @param presentMode The swap chain present mode. If the requested present mode is not supported, an exception is thrown.
		 */
		SwapChain(Device* pDevice, const std::shared_ptr<Display>& pDisplay, uint32_t imageCount, const SwapChainPresentMode presentMode);

		/**
		 * Recreate the swap chain.
		 * This is necessary if the display is resized.
		 */
		virtual void Recreate() = 0;

		/**
		 * Acquire the next swap chain image.
		 *
		 * @param frameIndex The frame index.
		 * @return The next image info structure.
		 */
		virtual NextImageInfo AcquireNextImage(const uint32_t frameIndex) = 0;

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
		const uint32_t GetImageCount() const { return mImageCount; }

		/**
		 * Get the current image index.
		 *
		 * @return The image index.
		 */
		const uint32_t GetImageIndex() const { return mImageIndex; }

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

		/**
		 * Toggle the recreate boolean.
		 */
		void ToggleRecreate() { bShouldRecreate = true; }

		/**
		 * Check if the swap chain should recreate.
		 * This occurs if the display is resized or due to some other issue. In that case, the swap chain should be created, and all the attached render targets.
		 *
		 * @return The boolean value.
		 */
		const bool ShouldRecreate() const { return bShouldRecreate; }

	protected:
		std::shared_ptr<Display> pDisplay = nullptr;

		FBox2D mExtent = {};
		uint32_t mImageCount = 0;
		uint32_t mImageIndex = 0;

		PixelFormat mPixelForamt = PixelFormat::Undefined;
		SwapChainPresentMode mPresentMode = SwapChainPresentMode::MailBox;

		bool bShouldRecreate = false;
	};
}