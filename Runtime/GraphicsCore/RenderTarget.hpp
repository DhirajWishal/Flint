// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBuffer.hpp"
#include "ComputePipeline.hpp"
#include "Core/CountingSemaphore.hpp"

namespace Flint
{
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
	template<class DeviceT, class ImageT>
	class RenderTarget : public DeviceBoundObject<DeviceT>
	{
	public:
		using RenderTargetAttachmentT = RenderTargetAttachment<ImageT>;

		/**
		 * Default constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param extent The render target extent.
		 * @param bufferCount The frame buffer count.
		 * @param imageAttachments The image attachments used by the render target.
		 */
		explicit RenderTarget(DeviceT* pDevice, const FBox2D& extent, const uint32_t bufferCount, const std::vector<RenderTargetAttachmentT>& imageAttachments)
			: DeviceBoundObject(pDevice), m_Extent(extent), m_BufferCount(bufferCount), m_Attachments(imageAttachments)
		{
			if (extent.IsZero())
				throw std::invalid_argument("Render target width and height should be greater than 0!");

			if (bufferCount == 0)
				throw std::invalid_argument("Render target buffer count should be greater than 0!");
		}

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
		const std::vector<RenderTargetAttachmentT> GetAttachments() const { return m_Attachments; }

		/**
		 * Get a single attachment from the attachments.
		 *
		 * @param index The index of the attachment.
		 * @return The attachment.
		 */
		const RenderTargetAttachmentT& GetAttachment(const uint64_t index) const { return m_Attachments[index]; }

		/**
		 * Get the current frame index.
		 *
		 * @return The frame index.
		 */
		const uint32_t GetFrameIndex() const { return m_FrameIndex; }

		/**
		 * Get the render target extent.
		 *
		 * @return The extent.
		 */
		const FBox2D GetExtent() const { return m_Extent; }

		/**
		 * Get the render target buffer count.
		 *
		 * @return The buffer count.
		 */
		const uint32_t GetBufferCount() const { return m_BufferCount; }

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
		void IncrementFrameIndex() { m_FrameIndex++; if (m_FrameIndex >= m_BufferCount) m_FrameIndex = 0; }

	protected:
		std::vector<RenderTargetAttachmentT> m_Attachments = {};

		FBox2D m_Extent = {};
		uint32_t m_BufferCount = 0;
		uint32_t m_FrameIndex = 0;

		bool bIsAltered = false;
	};
}