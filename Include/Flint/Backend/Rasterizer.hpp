// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"
#include "Types.hpp"

#include "RasterizingPipeline.hpp"

namespace Flint
{
	namespace Backend
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
			 * @param pDevice The device reference.
			 * @param camera The camera from which all the models are drawn from.
			 * @param frameCount The number of frames in the render target. This is usually set automatically by the Window.
			 * @param attachmentDescriptions The attachment descriptions.
			 * @param multisample The multisample count. Default is One.
			 * @param exclusiveBuffering Whether or not to use one buffer/ attachment per frame. Default is false.
			 */
			explicit Rasterizer(const std::shared_ptr<Device>& pDevice, Camera& camera, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions, Multisample multisample = Multisample::One, bool exclusiveBuffering = false)
				: RenderTarget(pDevice, camera, frameCount, std::move(attachmentDescriptions))
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

			/**
			 * Create a new rasterizing pipeline.
			 *
			 * @param pRasterizingProgram The rasterizing program pointer.
			 * @param specification The pipeline specification.
			 * @param pCacheHandler The pipeline cache handler. Default is nullptr.
			 * @return The created pipeline pointer.
			 */
			[[nodiscard]] virtual std::shared_ptr<RasterizingPipeline> createPipeline(const std::shared_ptr<RasterizingProgram>& pRasterizingProgram, const RasterizingPipelineSpecification& specification, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler = nullptr) = 0;

		protected:
			const Multisample m_Multisample = Multisample::One;
			const bool m_ExclusiveBuffering = false;
		};
	}
}