// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"
#include "Shader.hpp"
#include "RenderTarget.hpp"
#include "DynamicStateContainer.hpp"
#include "ResourceMap.hpp"

namespace Flint
{
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
		 * @param pDevice: The device pointer.
		 * @param pipelineName: The name of the pipeline.
		 * @param pComputeShader: The compute shader pointer.
		 */
		ComputePipeline(const std::shared_ptr<Device>& pDevice, const std::string& pipelineName, const std::shared_ptr<Shader>& pComputeShader);

	protected:
		std::shared_ptr<Shader> pShader = nullptr;
	};
}