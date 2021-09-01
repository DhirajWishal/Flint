// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/ComputePipeline.hpp"

namespace Flint
{
	ComputePipeline::ComputePipeline(const std::shared_ptr<Device>& pDevice, const std::string& pipelineName, const std::shared_ptr<Shader>& pComputeShader)
		: Pipeline(pDevice, pipelineName), pShader(pComputeShader)
	{
		if (!pShader)
			throw std::invalid_argument("Compute shader pointers should not be null!");
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
			throw std::invalid_argument("The provided ID is not present within the compute pipeline!");

		bShouldPrepareResources = true;
		mComputeInstances.erase(ID);
	}

	std::shared_ptr<ResourceMap> ComputePipeline::CreateResourceMap() const
	{
		std::vector<std::string> buffers;
		std::vector<std::string> images;

		const auto resources = pShader->GetShaderResources();
		for (const auto resource : resources)
		{
			if (resource.second.mType == ShaderResourceType::Sampler ||
				resource.second.mType == ShaderResourceType::SampledImage ||
				resource.second.mType == ShaderResourceType::StorageImage ||
				resource.second.mType == ShaderResourceType::CombinedImageSampler)
				INSERT_INTO_VECTOR(images, resource.first);
			else
				INSERT_INTO_VECTOR(buffers, resource.first);
		}

		return std::make_shared<ResourceMap>(buffers, images);
	}
}