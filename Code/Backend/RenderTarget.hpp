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
		RenderTarget(const std::shared_ptr<Device>& pDevice, const FExtent2D& extent, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pCommandBufferList)
			: DeviceBoundObject(pDevice), mExtent(extent), mBufferCount(bufferCount), pCommandBufferList(pCommandBufferList) {}

	protected:
		std::shared_ptr<CommandBufferList> pCommandBufferList = nullptr;

		FExtent2D mExtent = {};
		UI32 mBufferCount = 0;
	};
}
