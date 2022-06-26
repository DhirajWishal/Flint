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
		 * @param pDevice The device to which the ray tracer is bound to.
		 * @param camera The camera reference which is used to generate the images using.
		 * @param frameCount The number of frames to use.
		 */
		explicit RayTracer(const std::shared_ptr<Device>& pDevice, Camera& camera, uint32_t frameCount)
			: RenderTarget(pDevice, camera, frameCount, { Defaults::ColorAttachmentDescription }) {}

		/**
		 * Virtual default destructor.
		 */
		virtual ~RayTracer() = default;
	};
}