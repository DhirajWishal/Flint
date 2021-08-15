// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "OffScreenRenderTarget.hpp"

namespace Flint
{
	/**
	 * Flint off screen render target factory.
	 * This object enables the user to create multiple different types of off screen render targets.
	 */
	class OffScreenRenderTargetFactory : public FObject
	{
	public:
		/**
		 * Default constructor.
		 * 
		 * @param pDevice: The device pointer.
		 */
		OffScreenRenderTargetFactory(const std::shared_ptr<Device>& pDevice) : pDevice(pDevice) {}

		/**
		 * Create a new render target.
		 * 
		 * @param type: The type of the off screen render target.
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The number of buffers in the render target.
		 * @param threadCount: The number of threads to use. Default is 0.
		 * @return The render target pointer.
		 */
		virtual std::shared_ptr<OffScreenRenderTarget> Create(OffScreenRenderTargetType type, const FBox2D& extent, UI32 bufferCount, UI32 threadCount = 0) = 0;

	protected:
		std::shared_ptr<Device> pDevice = nullptr;
	};
}