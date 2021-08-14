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
		 * @param specifications: The render target attachments. Default is color and depth buffers.
		 * @param threadCount: The number of threads to use. Default is 0.
		 */
		OffScreenRenderTarget(
			const std::shared_ptr<Device>& pDevice,
			const FBox2D& extent,
			const UI32 bufferCount,
			const std::shared_ptr<CommandBufferList>& pCommandBufferList,
			const std::vector<OffScreenResultSpecification>& specifications = { OffScreenResultSpecification(OffScreenRenderTargetAttachment::COLOR_BUFFER, 1, PixelFormat::B8G8R8A8_SRGB), OffScreenResultSpecification(OffScreenRenderTargetAttachment::DEPTH_BUFFER, 1, PixelFormat::D32_SFLOAT) },
			UI32 threadCount = 0);

		/**
		 * Execute the render target.
		 * If a render target is attached, that render target is used to execute this render target.
		 *
		 * @param pScreenBoundRenderTarget: The screen bound render target pointer.
		 */
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

		/**
		 * Get the result specifications.
		 * 
		 * @return The vector of result specifications.
		 */
		const std::vector<OffScreenResultSpecification> GetResultSpecifications() const { return mResultSpecification; }

	protected:
		std::vector<std::shared_ptr<Image>> pResults = {};
		std::vector<OffScreenResultSpecification> mResultSpecification = {};
	};
}