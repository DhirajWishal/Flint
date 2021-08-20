// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ComputePipeline.hpp"

namespace Flint
{
	ComputePipeline::ComputePipeline(const std::shared_ptr<Device>& pDevice, const std::string& pipelineName, const std::shared_ptr<Shader>& pComputeShader)
		: Pipeline(pDevice, pipelineName), pShader(pComputeShader)
	{
		if (!pShader)
			FLINT_THROW_INVALID_ARGUMENT("Compute shader pointers should not be null!");
	}

	const UI64 ComputePipeline::AddInstance(const std::shared_ptr<ResourceMap>& pResourceMap, const std::shared_ptr<DynamicStateContainer>& pDynamicStates, const FBox3D& computeGroups)
	{
		mComputeInstances[mInstanceIndex] = ComputeInstance(pResourceMap, pDynamicStates, computeGroups);

		bShouldPrepareResources = true;
		return mInstanceIndex++;
	}

	void ComputePipeline::RemoveInstance(const UI64 ID)
	{
		if (mComputeInstances.find(ID) == mComputeInstances.end())
			FLINT_THROW_INVALID_ARGUMENT("The provided ID is not present within the compute pipeline!");

		bShouldPrepareResources = true;
		mComputeInstances.erase(ID);
	}
}