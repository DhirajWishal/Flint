// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FObject.h"
#include "Core\Backend\GraphicsPipeline.h"
#include "FBuffer.h"
#include "FShader.h"

namespace Flint
{
	class FScreenBoundRenderTarget;
	class FPipelineResource;

	/**
	 * Flint graphics pipeline.
	 */
	class FGraphicsPipeline final : public FObject {
	public:
		FGraphicsPipeline();
		~FGraphicsPipeline();

		/**
		 * Initialize the pipeline.
		 *
		 * @param renderTarget: The render target to which the pipeline is bound to.
		 * @param shaders: The shaders to be used.
		 * @param spec: The graphics specification of the pipeline.
		 */
		void Initialize(const FScreenBoundRenderTarget& renderTarget, const std::vector<FShader>& shaders, const GraphicsPipelineSpecification& spec);

		/**
		 * Terminate the pipeline.
		 */
		void Terminate();

	public:
		/**
		 * Create a new uniform buffer container.
		 * 
		 * @return The uniform buffer container.
		 */
		FUniformBufferContainer CreateUniformBufferContainer() const;

		/**
		 * Create a new pipeline resource object.
		 * 
		 * @return The created pipeline resource.
		 */
		FPipelineResource CreatePipelineResource() const;
	};
}