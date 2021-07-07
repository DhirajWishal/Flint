// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBufferList.hpp"

namespace Flint
{
	/**
	 * Flint render target object.
	 * This object is the base class for all the supported render targets.
	 *
	 * Render target objects are used to submit data to the device to be rendered or for compute purposes.
	 */
	class RenderTarget : public DeviceBoundObject
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice: The device pointer.
		 * @param extent: The render target extent.
		 * @param bufferCount: The frame buffer count.
		 * @param pCommandBufferList: The command buffer list used by the render target.
		 */
		RenderTarget(const std::shared_ptr<Device>& pDevice, const FExtent2D& extent, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pCommandBufferList)
			: DeviceBoundObject(pDevice), mExtent(extent), mBufferCount(bufferCount), pCommandBufferList(pCommandBufferList)
		{
			if (extent.mWidth == 0 || extent.mHeight == 0)
				FLINT_THROW_INVALID_ARGUMENT("Render target width and height should be greater than 0!");

			if (bufferCount == 0)
				FLINT_THROW_INVALID_ARGUMENT("Render target buffer count should be greater than 0!");

			if (!pCommandBufferList)
				FLINT_THROW_INVALID_ARGUMENT("Render target command buffer pointer should not be null!");
		}

	protected:
		std::shared_ptr<CommandBufferList> pCommandBufferList = nullptr;

		FExtent2D mExtent = {};
		UI32 mBufferCount = 0;
	};
}