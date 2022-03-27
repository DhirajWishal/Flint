// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/ComputePipeline.hpp"

namespace Flint
{
	ComputePipeline::ComputePipeline(Device* pDevice, const std::string& pipelineName, const std::shared_ptr<Shader>& pComputeShader)
		: Pipeline(pDevice, pipelineName), pShader(pComputeShader)
	{
		if (!pShader)
			throw std::invalid_argument("Compute shader pointers should not be null!");
	}
}