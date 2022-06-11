// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"

namespace Flint
{
	namespace Core
	{
		/**
		 * Ray tracer class.
		 * This class performs ray tracing to the bound entities.
		 */
		template<class TDevice, class TAttachment>
		class RayTracer : public RenderTarget<TDevice, TAttachment>
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device to which the ray tracer is bound to.
			 * @param camera The camera reference which is used to generate the images using.
			 * @param frameCount The number of frames to use.
			 * @param attachmentDescriptions The attachment descriptions to use.
			 */
			explicit RayTracer(TDevice& device, Camera& camera, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions)
				: RenderTarget<TDevice, TAttachment>(device, camera, frameCount, std::move(attachmentDescriptions)) {}

			/**
			 * Virtual default destructor.
			 */
			virtual ~RayTracer() = default;
		};
	}
}