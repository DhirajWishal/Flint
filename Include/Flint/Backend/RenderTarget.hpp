// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Graphical.hpp"
#include "RenderTargetAttachment.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Render target class.
		 * These are special objects, and entities are spawned into these. Once spawned, they are rendered to a custom texture which then can be used for other
		 * purposes.
		 */
		class RenderTarget : public DeviceBoundObject, public Graphical
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device reference.
			 * @param camera The camera from which all the models are drawn from.
			 * @param frameCount The number of frames in the render target. This is usually set automatically by the Window.
			 * @param attachmentDescriptions The attachment descriptions.
			 */
			explicit RenderTarget(const std::shared_ptr<Device>& pDevice, Camera& camera, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions)
				: DeviceBoundObject(pDevice)
				, Graphical(frameCount)
				, m_Camera(camera)
				, m_AttachmentDescriptions(std::move(attachmentDescriptions)) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~RenderTarget() = default;

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
			[[nodiscard]] virtual RenderTargetAttachment& getAttachment(uint32_t index) = 0;

			/**
			 * Get the render target attachment at a given index.
			 *
			 * @param index The index of the attachment.
			 * @return The attachment.
			 */
			[[nodiscard]] virtual const RenderTargetAttachment& getAttachment(uint32_t index) const = 0;

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

		protected:
			std::vector<AttachmentDescription> m_AttachmentDescriptions;

			Camera& m_Camera;
		};
	}
}