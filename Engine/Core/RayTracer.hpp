// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"

namespace Flint
{
	/**
	 * Ray tracer class.
	 * This class performs ray tracing to the bound entities.
	 */
	class RayTracer : public RenderTarget
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine reference.
		 * @param width The width of the render target.
		 * @param height The height of the render target.
		 * @param attachmentDescriptions The attachment descriptions.
		 * @param multisample The multisample count. Default is One.
		 */
		explicit RayTracer(Engine& engine, uint32_t width, uint32_t height, std::vector<AttachmentDescription>&& attachmentDescriptions, Multisample multisample = Multisample::One)
			: RenderTarget(engine, std::move(attachmentDescriptions), width, height, multisample) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~RayTracer() = default;
	};
}