// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"
#include "Display.hpp"

namespace Flint
{
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
		 * @param extent: The swap chain extent.
		 * @param mImageCount: The image count of the swap chain.
		 */
		SwapChain(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, UI32 mImageCount) : DeviceBoundObject(pDevice) {}

		/**
		 * Recreate the swap chain.
		 * This is necessary if the display is resized.
		 */
		virtual void Recreate() = 0;

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

	protected:
		std::shared_ptr<Display> pDisplay = nullptr;

		FBox2D mExtent = {};
		UI32 mImageCount = 0;

		PixelFormat mPixelForamt = PixelFormat::UNDEFINED; 
	};
}