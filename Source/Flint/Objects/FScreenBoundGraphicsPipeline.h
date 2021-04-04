// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FObject.h"
#include "Core\Backend\GraphicsPipeline.h"

namespace Flint
{
	static constexpr const UI64 GetScreenBoundGraphicsPipelineSize();
	static constexpr const UI64 GetScreenBoundGraphicsPipelineAlignment();

	class FScreenBoundRenderTarget;

	/**
	 * Flint screen bound graphics pipeline.
	 * This pipeline is compatible only with the screen bound render target.
	 */
	class FScreenBoundGraphicsPipeline final : public FObject<GetScreenBoundGraphicsPipelineSize(), GetScreenBoundGraphicsPipelineAlignment()> {
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