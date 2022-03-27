// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/ComputePipeline.hpp"

namespace Flint
{
	ComputePipeline::ComputePipeline(Device* pDevice, const std::string& pipelineName, std::unique_ptr<Shader>&& pComputeShader)
		: Pipeline(pDevice, pipelineName), pShader(std::move(pComputeShader))
	{
		if (!pShader)
			throw std::invalid_argument("Compute shader pointers should not be null!");
	}
}