// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FObject.h"
#include "Core\Backend\GraphicsPipeline.h"

#if defined(FLINT_BACKEND_VULKAN)
#define FLINT_SB_GRAPHICS_PIPELINE_BACKEND_SIZE		696Ui64

#endif // defined(FLINT_BACKEND_VULKAN)

namespace Flint
{
	class FScreenBoundRenderTarget;

	/**
	 * Flint screen bound graphics pipeline.
	 * This pipeline is compatible only with the screen bound render target.
	 */
	class FScreenBoundGraphicsPipeline final : public FObject<FLINT_SB_GRAPHICS_PIPELINE_BACKEND_SIZE> {
	public:
		FScreenBoundGraphicsPipeline();
		~FScreenBoundGraphicsPipeline();

		/**
		 * Initialize the pipeline.
		 * 
		 * @param renderTarget: The render target to which the pipeline is bound to.
		 * @param shaders: The shaders to be used.
		 * @param spec: The graphics specification of the pipeline.
		 */
		void Initialize(const FScreenBoundRenderTarget& renderTarget, const std::vector<ShaderDigest>& shaders, const Backend::GraphicsPipelineSpecification& spec);

		/**
		 * Terminate the pipeline.
		 */
		void Terminate();
	};
}