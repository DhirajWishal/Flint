// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"
#include "Image.hpp"

namespace Flint
{
	class ScreenBoundRenderTarget;

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
		 * @param pDevice: The device pointer.
		 * @param extent: The frame buffer extent.
		 * @param bufferCount: The frame buffer count.
		 * @param pCommandBufferList: The command buffer list to be used.
		 * @param attachments: The render target attachments. Default is OffScreenRenderTargetAttachment::COLOR_BUFFER | OffScreenRenderTargetAttachment::DEPTH_BUFFER.
		 * @param threadCount: The number of threads to use. Default is 0.
		 */
		OffScreenRenderTarget(
			const std::shared_ptr<Device>& pDevice,
			const FBox2D& extent,
			const UI32 bufferCount,
			const std::shared_ptr<CommandBufferList>& pCommandBufferList,
			OffScreenRenderTargetAttachment attachments = OffScreenRenderTargetAttachment::COLOR_BUFFER | OffScreenRenderTargetAttachment::DEPTH_BUFFER,
			UI32 threadCount = 0);

		virtual void Execute(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget = nullptr) = 0;

	public:
		/**
		 * Get the result images from the render target.
		 * 
		 * @return The result image pointers.
		 */
		const std::vector<std::shared_ptr<Image>> GetResults() const { return pResults; }

		/**
		 * Get a single result image from the render target.
		 * 
		 * @return The result image pointer.
		 */
		const std::shared_ptr<Image> GetResult(const UI64 index) const { return pResults[index]; }

	protected:
		std::vector<std::shared_ptr<Image>> pResults = {};

		OffScreenRenderTargetAttachment mAttachments = OffScreenRenderTargetAttachment(0);
	};
}