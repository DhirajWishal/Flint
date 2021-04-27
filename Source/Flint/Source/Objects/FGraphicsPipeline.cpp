// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Objects\FGraphicsPipeline.h"
#include "Flint\Objects\FScreenBoundRenderTarget.h"
#include "Flint\Objects\FPipelineResource.h"

#if defined(FLINT_BACKEND_VULKAN)
#include "VulkanBackend\RenderTargets\Pipelines\VulkanGraphicsPipeline.h"
typedef Flint::VulkanBackend::VulkanGraphicsPipeline Pipeline;
typedef Flint::VulkanBackend::VulkanPipelineResource Resource;

#elif defined(FLINT_BACKEND_DIRECT_X_12)

#elif defined(FLINT_BACKEND_WEB_GPU)

#else
error "Selected backend is invalid! Make sure that you select a proper supported backend!"

#endif // defined(FLINT_BACKEND_VULKAN)


namespace Flint
{
	FGraphicsPipeline::FGraphicsPipeline()
	{
		Construct<Pipeline>();
	}

	FGraphicsPipeline::~FGraphicsPipeline()
	{
	}

	void FGraphicsPipeline::Initialize(const FScreenBoundRenderTarget& renderTarget, const std::vector<FShader>& shaders, const GraphicsPipelineSpecification& spec)
	{
		std::vector<ShaderDigest> digests(shaders.size());
		for (UI8 itr = 0; itr < shaders.size(); itr++)
			digests[itr] = shaders[itr].GetShaderDigest();

		GetAs<Pipeline>().Initialize(static_cast<VulkanBackend::VulkanScreenBoundRenderTargetS*>(renderTarget.GetBackendObject().get()), digests, spec);
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
				buffer.Initialize(FDevice(std::shared_ptr<BackendObject>(GetAs<Pipeline>().GetRenderTarget()->GetDevice())), itr.second.mSize, BufferUsage::UNIFORM, MemoryProfile::TRANSFER_FRIENDLY);
				container[itr.first] = std::move(buffer);
			}
		}

		return container;
	}

	FPipelineResource FGraphicsPipeline::CreatePipelineResource() const
	{
		std::shared_ptr<Resource> pResource = std::make_shared<Resource>();
		*pResource = GetAs<Pipeline>().CreatePipelineResource();

		return FPipelineResource(pResource);
	}
}