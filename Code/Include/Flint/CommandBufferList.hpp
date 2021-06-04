// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Flint command buffer list object.
	 * This object stored multiple command buffers which are used to submit commands to the GPU.
	 */
	class FLINT_API CommandBufferList : public DeviceBoundObject
	{
	public:
		/**
		 * Construct the command buffer list as a primary command buffer list.
		 *
		 * @param device: The device to which the buffer list is bound to.
		 * @param bufferCount: The number of command buffers needed.
		 */
		CommandBufferList(Device& device, const UI32 bufferCount) : DeviceBoundObject(device), mBufferCount(bufferCount) {}

		/**
		 * Construct the command buffer list as a secondary command buffer list.
		 * Secondary command buffer lists are used to multi-thread and to bind objects per frame.
		 *
		 * @param device: The device object to which the buffer is bound to.
		 * @param bufferCount: The number of buffers needed.
		 * @param parent: The parent command buffer list.
		 */
		CommandBufferList(Device& device, const UI32 bufferCount, CommandBufferList& parent) : DeviceBoundObject(device), mBufferCount(bufferCount) {}

	protected:
		UI32 mBufferCount = 0;
	};
}