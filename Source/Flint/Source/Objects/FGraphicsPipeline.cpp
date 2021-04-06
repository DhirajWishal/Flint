// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Objects\FGraphicsPipeline.h"
#include "Flint\Objects\FScreenBoundRenderTarget.h"
#include "Flint\Objects\FPipelineResource.h"

#if defined(FLINT_BACKEND_VULKAN)
#include "VulkanBackend\RenderTargets\Pipelines\VulkanGraphicsPipeline.h"
typedef Flint::VulkanBackend::VulkanGraphicsPipeline Pipeline;
typedef Flint::VulkanBackend::VulkanPipelineResource Resource;

#endif // defined(FLINT_BACKEND_VULKAN)


namespace Flint
{
	FGraphicsPipeline::FGraphicsPipeline()
	{
		Construct<Pipeline>();
	}

	FGraphicsPipeline::~FGraphicsPipeline()
	{
		Destruct<Pipeline>();
	}

	void FGraphicsPipeline::Initialize(const FScreenBoundRenderTarget& renderTarget, const std::vector<ShaderDigest>& shaders, const Backend::GraphicsPipelineSpecification& spec)
	{
		GetAs<Pipeline>().Initialize(static_cast<VulkanBackend::VulkanScreenBoundRenderTargetS*>(renderTarget.GetBackendObject()), shaders, spec);
	}

	void FGraphicsPipeline::Terminate()
	{
		GetAs<Pipeline>().Terminate();
	}

	FUniformBufferContainer FGraphicsPipeline::CreateUniformBufferContainer() const
	{
		auto uniformLayouts = GetAs<Pipeline>().GetUniformLayouts();

		FUniformBufferContainer container = {};
		for (auto& itr : uniformLayouts)
		{
			if (itr.second.mType == UniformType::STORAGE_BUFFER || itr.second.mType == UniformType::STORAGE_BUFFER_DYNAMIC || itr.second.mType == UniformType::STORAGE_TEXEL_BUFFER ||
				itr.second.mType == UniformType::UNIFORM_BUFFER || itr.second.mType == UniformType::UNIFORM_BUFFER_DYNAMIC || itr.second.mType == UniformType::UNIFORM_TEXEL_BUFFER)
			{
				FBuffer buffer = {};
				buffer.Initialize(FDevice(GetAs<Pipeline>().GetRenderTarget()->GetDevice()), itr.second.mSize, Backend::BufferUsage::UNIFORM, Backend::MemoryProfile::TRANSFER_FRIENDLY);
				container[itr.first] = std::move(buffer);
			}
		}

		return container;
	}
	
	FPipelineResource FGraphicsPipeline::CreatePipelineResource() const
	{
		Resource* pResource = new Resource();
		*pResource = GetAs<Pipeline>().CreatePipelineResource();

		return FPipelineResource(pResource);
	}
}