// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTargetAttachment.hpp"
#include "Camera/Camera.hpp"

namespace Flint
{
	namespace Core
	{
		/**
		 * Render target class.
		 * These are special objects, and entities are spawned into these. Once spawned, they are rendered to a custom texture which then can be used for other
		 * purposes.
		 */
		template<class TDevice, class TAttachment>
		class RenderTarget : public DeviceBoundObject<TDevice>
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device reference.
			 * @param camera The camera from which all the models are drawn from.
			 * @param frameCount The number of frames in the render target. This is usually set automatically by the Window.
			 * @param attachmentDescriptions The attachment descriptions.
			 */
			explicit RenderTarget(TDevice& device, Camera& camera, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions)
				: DeviceBoundObject<TDevice>(device)
				, m_Camera(camera)
				, m_FrameCount(frameCount)
				, m_AttachmentDescriptions(std::move(attachmentDescriptions)) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~RenderTarget() = default;

			/**
			 * Update the render target.
			 * This will perform whatever operation that should be done.
			 */
			virtual void update() = 0;

			/**
			 * Update the render target extent.
			 * This needs to be called if the camera's frame extents are altered.
			 */
			virtual void updateExtent() = 0;

			/**
			 * Get the render target attachment at a given index.
			 *
			 * @param index The index of the attachment.
			 * @return The attachment.
			 */
			[[nodiscard]] virtual TAttachment& getAttachment(uint32_t index) = 0;

			/**
			 * Get the render target attachment at a given index.
			 *
			 * @param index The index of the attachment.
			 * @return The attachment.
			 */
			[[nodiscard]] virtual const TAttachment& getAttachment(uint32_t index) const = 0;

			/**
			 * Get the width of the render target.
			 *
			 * @return The width.
			 */
			[[nodiscard]] uint32_t getWidth() const { return m_Camera.getFrameWidth(); }

			/**
			 * Get the height of the render target.
			 *
			 * @return The height.
			 */
			[[nodiscard]] uint32_t getHeight() const { return m_Camera.getFrameHeight(); }

			/**
			 * Get the frame count.
			 *
			 * @return The frame count.
			 */
			[[nodsicard]] uint32_t getFrameCount() const { return m_FrameCount; }

			/**
			 * Get the current frame index.
			 *
			 * @return The frame index.
			 */
			[[nodsicard]] uint32_t getFrameIndex() const { return m_FrameIndex; }

		protected:
			/**
			 * Increment the frame index to the next one.
			 */
			void incrementFrameIndex() { m_FrameIndex = ++m_FrameIndex % m_FrameCount; }

		protected:
			std::vector<AttachmentDescription> m_AttachmentDescriptions;

			Camera& m_Camera;

			const uint32_t m_FrameCount = 0;
			uint32_t m_FrameIndex = 0;
		};
	}
}