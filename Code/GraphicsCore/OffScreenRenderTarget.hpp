// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"
#include "Image.hpp"

namespace Flint
{
	/**
	 * Flint off screen render target.
	 * Off screen render targets are used for computational and general off screen processes like shadow mapping.
	 *
	 * Note: If the render target is intended to be attached to a screen bound render target, make sure to have the same number of buffers as in the screen bound render target.
	 */
	class OffScreenRenderTarget : public RenderTarget
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param extent The frame buffer extent.
		 * @param bufferCount The frame buffer count.
		 * @param imageAttachments The image attachments used in the render target.
		 */
		OffScreenRenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments);

		/**
		 * Recreate the off screen render target.
		 * 
		 * @param extent The new extent.
		 */
		virtual void Recreate(const FBox2D& extent) = 0;
	};
}