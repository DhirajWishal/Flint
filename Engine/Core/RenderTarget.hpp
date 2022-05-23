// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTargetAttachment.hpp"
#include "Geometry.hpp"

namespace Flint
{
	/**
	 * Render target class.
	 * These are special objects, and entities are spawned into these. Once spawned, they are rendered to a custom texture which then can be used for other
	 * purposes.
	 */
	class RenderTarget : public EngineBoundObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine reference.
		 * @param width The width of the render target.
		 * @param height The height of the render target.
		 * @param frameCount The number of frames in the render target. This is usually set automatically by the Window.
		 * @param attachmentDescriptions The attachment descriptions.
		 * @param multisample The multisample count. Default is One.
		 * @param exclusiveBuffering Whether or not to use one buffer/ attachment per frame. Default is false.
		 */
		explicit RenderTarget(Engine& engine, uint32_t width, uint32_t height, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions, Multisample multisample = Multisample::One, bool exclusiveBuffering = false)
			: EngineBoundObject(engine), m_AttachmentDescriptions(std::move(attachmentDescriptions)), m_Width(width), m_Height(height), m_FrameCount(frameCount), m_Multisample(multisample), m_ExclusiveBuffering(exclusiveBuffering) {}

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
		 * Resize the render target.
		 *
		 * @param width The width of the render target.
		 * @param height The height of the render target.
		 */
		virtual void resize(uint32_t width, uint32_t height) = 0;

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
		[[nodiscard]] uint32_t getWidth() const { return m_Width; }

		/**
		 * Get the height of the render target.
		 *
		 * @return The height.
		 */
		[[nodiscard]] uint32_t getHeight() const { return m_Height; }

		/**
		 * Get the number of frames in the render target.
		 *
		 * @return The frame count.
		 */
		[[nodiscard]] uint32_t getFrameCount() const { return m_FrameCount; }

		/**
		 * Get the multisample count.
		 *
		 * @return The multisample count.
		 */
		[[nodiscard]] Multisample getMultisample() const { return m_Multisample; }

	protected:
		/**
		 * Increment the frame index to the next one.
		 */
		void incrementFrameIndex() { m_FrameIndex = ++m_FrameIndex % m_FrameCount; }

	protected:
		std::vector<AttachmentDescription> m_AttachmentDescriptions;
		std::vector<Geometry> m_Geometries;

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		const uint32_t m_FrameCount = 0;
		uint32_t m_FrameIndex = 0;

		const Multisample m_Multisample = Multisample::One;
		const bool m_ExclusiveBuffering = false;
	};
}