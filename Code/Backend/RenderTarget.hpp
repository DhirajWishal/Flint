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
		RenderTarget(const std::shared_ptr<Device>& pDevice, const FExtent2D& extent, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pCommandBufferList);

		/**
		 * Prepare the static resources to draw.
		 * This will bind all of the current content to the command buffer to be drawn.
		 */
		virtual void PrepareStaticResources() = 0;

		/**
		 * Begin a new frame.
		 * This call prepares resources for a new draw call and must be called prior to updating resources.
		 */
		virtual void BeginFrame() = 0;

		/**
		 * Update the resources.
		 * This call updates draw resources.
		 */
		virtual void Update() = 0;

		/**
		 * Submit the frame to the device.
		 * This call draws data to the screen by submitting the draw call to the device.
		 */
		virtual void SubmitFrame() = 0;

		/**
		 * Get the render target extent.
		 *
		 * @return The extent.
		 */
		FExtent2D GetExtent() const { return mExtent; }

		/**
		 * Get the render target buffer count.
		 *
		 * @return The buffer count.
		 */
		UI32 GetBufferCount() const { return mBufferCount; }

	protected:
		std::shared_ptr<CommandBufferList> pCommandBufferList = nullptr;

		FExtent2D mExtent = {};
		UI32 mBufferCount = 0;
	};
}