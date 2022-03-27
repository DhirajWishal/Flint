// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"
#include "Shader.hpp"
#include "DynamicStateContainer.hpp"

namespace Flint
{
	/**
	 * Flint compute pipeline.
	 * This pipeline is used to perform compute tasks on the GPU.
	 */
	template<class DeviceT, class RenderTargetT, class ResourcePackageT, class ShaderT>
	class ComputePipeline : public Pipeline<DeviceT, RenderTargetT, ResourcePackageT>
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param pipelineName The name of the pipeline.
		 * @param pComputeShader The compute shader pointer.
		 */
		ComputePipeline(DeviceT* pDevice, const std::string& pipelineName, std::unique_ptr<ShaderT>&& pComputeShader)
			: Pipeline(pDevice, pipelineName), pShader(std::move(pComputeShader))
		{
			if (!pShader)
				throw std::invalid_argument("Compute shader pointers should not be null!");
		}

	public:
		/**
		 * Get the compute shader.
		 *
		 * @return The shader pointer.
		 */
		ShaderT* GetShader() const { return pShader.get(); }

	protected:
		std::unique_ptr<ShaderT> pShader = nullptr;

		uint64_t mInstanceIndex = 0;
	};
}