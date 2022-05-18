// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTargetAttachment.hpp"

#include <vector>

namespace Flint
{
	/**
	 * Attachment description structure.
	 * This structure describes information about a single render target attachment.
	 */
	struct AttachmentDescription final
	{
		PixelFormat m_Format = PixelFormat::Undefined;	// If set to undefined, the best suitable will be set automatically.
		AttachmentType m_Type = AttachmentType::Color;
	};

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
		 */
		explicit RenderTarget(Engine& engine, uint32_t width, uint32_t height, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions, Multisample multisample = Multisample::One)
			: EngineBoundObject(engine), m_AttachmentDescriptions(std::move(attachmentDescriptions)), m_Width(width), m_Height(height), m_FrameCount(frameCount), m_Multisample(multisample) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~RenderTarget() = default;

		/**
		 * Get the render target attachment at a given index.
		 *
		 * @param index The index of the attachment.
		 * @return The attachment.
		 */
		[[nodiscard]] RenderTargetAttachment& getAttachment(uint32_t index) { return *m_pAttachments[index].get(); }

		/**
		 * Get the render target attachment at a given index.
		 *
		 * @param index The index of the attachment.
		 * @return The attachment.
		 */
		[[nodiscard]] const RenderTargetAttachment& getAttachment(uint32_t index) const { return *m_pAttachments[index].get(); }

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

	protected:
		std::vector<AttachmentDescription> m_AttachmentDescriptions;
		std::vector<std::unique_ptr<RenderTargetAttachment>> m_pAttachments;

	private:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		const uint32_t m_FrameCount = 0;

		const Multisample m_Multisample = Multisample::One;
	};
}