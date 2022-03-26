// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBuffer.hpp"
#include "ComputePipeline.hpp"
#include "Core/CountingSemaphore.hpp"

namespace Flint
{
	class GraphicsPipeline;
	class GeometryStore;
	class CommandBuffer;

	/**
	 * Flint render target object.
	 * This object is the base class for all the supported render targets.
	 *
	 * Render target objects are used to submit data to the device to be rendered or for compute purposes.
	 *
	 * Render targets can use secondary threads to speed up rendering. By default, the number of threads used per render target is 0, though it can be set to a specified value.
	 * Load balancing is done automatically by the render target using the following equation.
	 *		Load Per Thread = Draw Instances / Thread Count
	 * Each thread contains its own pair of GeometryStore and pipelines. So we recommend having one thread per geometry store in use IF the pipeline count or draw data count is high.
	 */
	class RenderTarget : public DeviceBoundObject
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param extent The render target extent.
		 * @param bufferCount The frame buffer count.
		 * @param imageAttachments The image attachments used by the render target.
		 */
		RenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const uint32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments);

		/**
		 * Prepare all the resources for a new frame.
		 * 
		 * @return IF successfully prepared, it returns a true, if not a false.
		 */
		virtual bool PrepareNewFrame() = 0;

	public:
		/**
		 * Get the attachments of the render target.
		 *
		 * @return The attachments.
		 */
		const std::vector<RenderTargetAttachment> GetAttachments() const { return mAttachments; }

		/**
		 * Get a single attachment from the attachments.
		 *
		 * @param index The index of the attachment.
		 * @return The attachment.
		 */
		const RenderTargetAttachment GetAttachment(const uint64 index) const { return mAttachments[index]; }

		/**
		 * Get the current frame index.
		 *
		 * @return The frame index.
		 */
		const uint32 GetFrameIndex() const { return mFrameIndex; }

		/**
		 * Get the render target extent.
		 *
		 * @return The extent.
		 */
		const FBox2D GetExtent() const { return mExtent; }

		/**
		 * Get the render target buffer count.
		 *
		 * @return The buffer count.
		 */
		const uint32 GetBufferCount() const { return mBufferCount; }

		/**
		 * Check if the render target is altered.
		 *
		 * @return Boolean value.
		 */
		const bool IsAltered() const { return bIsAltered; }

		/**
		 * Flag the render target that an alteration is made.
		 */
		void FlagAltered() { bIsAltered = true; }

		/**
		 * Increment the frame index.
		 * If the frame index is bigger than or equal to the buffer count, it is rolled back to 0.
		 */
		void IncrementFrameIndex() { mFrameIndex++; if (mFrameIndex >= mBufferCount) mFrameIndex = 0; }

	protected:
		std::vector<RenderTargetAttachment> mAttachments = {};

		FBox2D mExtent = {};
		uint32 mBufferCount = 0;
		uint32 mFrameIndex = 0;

		bool bIsAltered = false;
	};
}