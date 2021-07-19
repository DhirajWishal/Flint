// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	class ScreenBoundRenderTarget;

	/**
	 * Flint command buffer list object.
	 * This object stored multiple command buffers which are used to submit commands to the GPU.
	 */
	class CommandBufferList : public DeviceBoundObject
	{
	public:
		/**
		 * Construct the command buffer list as a primary command buffer list.
		 *
		 * @param pDevice: The device to which the buffer list is bound to.
		 * @param bufferCount: The number of command buffers needed.
		 */
		CommandBufferList(const std::shared_ptr<Device>& pDevice, const UI32 bufferCount) : DeviceBoundObject(pDevice), mBufferCount(bufferCount)
		{
			if (bufferCount == 0)
				FLINT_THROW_INVALID_ARGUMENT("Command buffer count should be greater than 0!");
		}

		/**
		 * Construct the command buffer list as a secondary command buffer list.
		 * Secondary command buffer lists are used to multi-thread and to bind objects per frame.
		 *
		 * @param pDevice: The device object to which the buffer is bound to.
		 * @param bufferCount: The number of buffers needed.
		 * @param pParent: The parent command buffer list.
		 */
		CommandBufferList(const std::shared_ptr<Device>& pDevice, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pParent) : DeviceBoundObject(pDevice), mBufferCount(bufferCount) {}

		/**
		 * Begin command buffer recording.
		 * Before binding objects to a command buffer, it must first start recording.
		 *
		 * @param index: The index of the command buffer.
		 */
		virtual void BeginBufferRecording(UI32 index) = 0;

		/**
		 * Begin secondary command buffer recording.
		 * Before binding objects to a command buffer, it must first start recording.
		 *
		 * @param index: The index of the command buffer.
		 * @param pParent: The parent command buffer pointer.
		 */
		virtual void BeginBufferRecording(UI32 index, const std::shared_ptr<CommandBufferList> pParent) = 0;

		/**
		 * Begin the next buffer recording.
		 */
		virtual void BeginNextBufferRecording() = 0;

		/**
		 * Begin the next buffer recording.
		 *
		 * @param pParent: The parent command buffer pointer.
		 */
		virtual void BeginNextBufferRecording(const std::shared_ptr<CommandBufferList> pParent) = 0;

		/**
		 * Bind a render target to the current command buffer.
		 *
		 * @param pRenderTarget: The render target pointer.
		 */
		virtual void BindRenderTarget(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget) = 0;

		/**
		 * Unbind the render target from the current buffer.
		 */
		virtual void UnbindRenderTarget() = 0;

		/**
		 * End the currently active command buffer recording.
		 */
		virtual void EndBufferRecording() = 0;

		/**
		 * Clear the command buffers.
		 */
		virtual void ClearBuffers() = 0;

		/**
		 * Get the command buffer list buffer count.
		 *
		 * @return The buffer count.
		 */
		UI32 GetBufferCount() const { return mBufferCount; }

		/**
		 * Get the current buffer index.
		 *
		 * @return The index of the currently selected buffer.
		 */
		UI32 GetCurrentBufferIndex() const { return mCurrentBufferIndex; }

	protected:
		UI32 mBufferCount = 0;
		UI32 mCurrentBufferIndex = 0;
	};
}