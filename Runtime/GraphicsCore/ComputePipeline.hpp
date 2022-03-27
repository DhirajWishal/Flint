// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"
#include "Shader.hpp"
#include "DynamicStateContainer.hpp"

namespace Flint
{
	class ScreenBoundRenderTarget;
	class OffScreenRenderTarget;

	/**
	 * Flint compute pipeline.
	 * This pipeline is used to perform compute tasks on the GPU.
	 */
	class ComputePipeline : public Pipeline
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param pipelineName The name of the pipeline.
		 * @param pComputeShader The compute shader pointer.
		 */
		ComputePipeline(const std::shared_ptr<Device>& pDevice, const std::string& pipelineName, const std::shared_ptr<Shader>& pComputeShader);

	public:
		/**
		 * Get the compute shader.
		 *
		 * @return The shader pointer.
		 */
		const std::shared_ptr<Shader> GetShader() const { return pShader; }

	protected:
		std::shared_ptr<Shader> pShader = nullptr;

		uint64_t mInstanceIndex = 0;
	};
}