// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"

namespace Flint
{
	/**
	 * Rasterizer class.
	 * This class performs rasterization to the bound entities.
	 */
	class Rasterizer : public RenderTarget
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
		explicit Rasterizer(Engine& engine, uint32_t width, uint32_t height, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions, Multisample multisample = Multisample::One)
			: RenderTarget(engine, width, height, frameCount, std::move(attachmentDescriptions), multisample) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~Rasterizer() = default;
	};
}