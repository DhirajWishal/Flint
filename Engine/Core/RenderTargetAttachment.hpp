// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	/**
	 * Render target attachment class.
	 * This class contains information about a single render target attachment image.
	 */
	class RenderTargetAttachment : public DeviceBoundObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pDevice The device reference.
		 * @param width The width of the attachment.
		 * @param height The height of the attachment.
		 * @param type The type of the attachment.
		 * @param format The format of the attachment.
		 * @param multisample The multisample count to use.
		 */
		explicit RenderTargetAttachment(const std::shared_ptr<Device>& pDevice, uint32_t width, uint32_t height, AttachmentType type, PixelFormat format, Multisample multisample);

		/**
		 * Default virtual destructor.
		 */
		virtual ~RenderTargetAttachment() = default;

		/**
		 * Get the width of the image.
		 *
		 * @return The width.
		 */
		[[nodiscard]] uint32_t getWidth() const { return m_Width; }

		/**
		 * Get the height of the image.
		 *
		 * @return The height.
		 */
		[[nodiscard]] uint32_t getHeight() const { return m_Height; }

		/**
		 * Get the attachment type.
		 *
		 * @return The attachment type.
		 */
		[[nodiscard]] AttachmentType getType() const { return m_Type; }

		/**
		 * Get the attachment's pixel format.
		 *
		 * @return The format.
		 */
		[[nodiscard]] PixelFormat getFormat() const { return m_Format; }

		/**
		 * Get the attachment's multisample count.
		 *
		 * @return The multisample count.
		 */
		[[nodiscard]] Multisample getMultisample() const { return m_Multisample; }

	protected:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		const AttachmentType m_Type = AttachmentType::Color;
		const PixelFormat m_Format = PixelFormat::Undefined;
		const Multisample m_Multisample = Multisample::One;
	};
}