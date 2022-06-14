// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"
#include "Types.hpp"

#include <functional>

namespace Flint
{
	namespace Core
	{
		/**
		 * Rasterizer class.
		 * This class performs rasterization to the bound entities.
		 */
		template<class TDevice, class TAttachment>
		class Rasterizer : public RenderTarget<TDevice, TAttachment>
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device reference.
			 * @param camera The camera from which all the models are drawn from.
			 * @param frameCount The number of frames in the render target. This is usually set automatically by the Window.
			 * @param attachmentDescriptions The attachment descriptions.
			 * @param multisample The multisample count. Default is One.
			 * @param exclusiveBuffering Whether or not to use one buffer/ attachment per frame. Default is false.
			 */
			explicit Rasterizer(TDevice& device, Camera& camera, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions, Multisample multisample = Multisample::One, bool exclusiveBuffering = false)
				: RenderTarget<TDevice, TAttachment>(device, camera, frameCount, std::move(attachmentDescriptions))
				, m_Multisample(multisample)
				, m_ExclusiveBuffering(exclusiveBuffering) {}

			/**
			 * Virtual default destructor.
			 */
			virtual ~Rasterizer() = default;

			/**
			 * Get the multisample count.
			 *
			 * @return The multisample count.
			 */
			[[nodiscard]] Multisample getMultisample() const { return m_Multisample; }

		protected:
			const Multisample m_Multisample = Multisample::One;
			const bool m_ExclusiveBuffering = false;
		};
	}
}