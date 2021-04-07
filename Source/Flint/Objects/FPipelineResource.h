// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FGraphicsPipeline.h"

namespace Flint
{
	/**
	 * Flint pipeline resource.
	 *
	 * This object is used to submit resources to the pipeline at draw time.
	 */
	class FPipelineResource final : public FObject {
	public:
		FPipelineResource();
		explicit FPipelineResource(Backend::BackendObject* pBackendObject);
		~FPipelineResource();

		/**
		 * Initialize pipeline resource.
		 * 
		 * @param pipeline: The pipeline which the resource is bound to.
		 */
		void Initialize(const FGraphicsPipeline& pipeline);

		/**
		 * Terminate the pipeline resource.
		 */
		void Terminate();

		/**
		 * Register the uniform buffer container.
		 * 
		 * @param container: The uniform buffer container to be registered.
		 */
		void RegisterUniformBufferContainer(const FUniformBufferContainer& container);

	private:
		bool bShouldDelete = false;
	};
}