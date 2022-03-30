// Copyright 2021-2022 Dhiraj Wishal
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
	template<class DeviceT, class ImageT>
	class OffScreenRenderTarget : public RenderTarget<DeviceT, ImageT>
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
		explicit OffScreenRenderTarget(DeviceT* pDevice, const FBox2D& extent, const uint32_t bufferCount, const std::vector<typename RenderTarget<DeviceT, ImageT>::RenderTargetAttachmentT>& imageAttachments)
			: RenderTarget(pDevice, extent, bufferCount, imageAttachments)
		{
		}

		/**
		 * Recreate the off screen render target.
		 * 
		 * @param extent The new extent.
		 */
		virtual void Recreate(const FBox2D& extent) = 0;
	};
}