// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Objects\FPipelineResource.h"

#if defined(FLINT_BACKEND_VULKAN)
#include "VulkanBackend\RenderTargets\Pipelines\VulkanPipeline.h"
#include "VulkanBackend\RenderTargets\Pipelines\VulkanGraphicsPipeline.h"
typedef Flint::VulkanBackend::VulkanPipelineResource Resource;
typedef Flint::VulkanBackend::VulkanGraphicsPipeline GraphicsPipeline;
typedef Flint::VulkanBackend::VulkanBuffer Buffer;

#endif // defined(FLINT_BACKEND_VULKAN)


namespace Flint
{
	FPipelineResource::FPipelineResource()
	{
		Construct<Resource>();
		bShouldDelete = true;
	}

	FPipelineResource::FPipelineResource(Backend::BackendObject* pBackendObject)
	{
		this->pBackendObject = pBackendObject;
	}

	FPipelineResource::~FPipelineResource()
	{
		if (bShouldDelete)
			Destruct<Resource>();
	}

	void FPipelineResource::Initialize(const FGraphicsPipeline& pipeline)
	{
		GetAs<Resource>().Initialize(static_cast<GraphicsPipeline*>(pipeline.GetBackendObject()));
	}

	void FPipelineResource::Terminate()
	{
		GetAs<Resource>().Terminate();
	}

	void FPipelineResource::RegisterUniformBufferContainer(const FUniformBufferContainer& container)
	{
		std::unordered_map<String, Buffer> uniformBuffers;
		for (auto& itr : container)
			uniformBuffers[itr.first] = static_cast<Buffer&>(*itr.second.GetBackendObject());

		GetAs<Resource>().RegisterUniformBuffers(uniformBuffers);
	}
}