// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Objects\FScreenBoundGraphicsPipeline.h"
#include "Flint\Objects\FScreenBoundRenderTarget.h"

#if defined(FLINT_BACKEND_VULKAN)
#include "VulkanBackend\RenderTargets\VulkanScreenBoundRenderTargetS.h"
#include "VulkanBackend\RenderTargets\Pipelines\VulkanGraphicsPipeline.h"
typedef Flint::VulkanBackend::VulkanGraphicsPipeline<Flint::VulkanBackend::VulkanScreenBoundRenderTargetS> Pipeline;

#endif // defined(FLINT_BACKEND_VULKAN)


namespace Flint
{
	FScreenBoundGraphicsPipeline::FScreenBoundGraphicsPipeline()
	{
		Construct<Pipeline>();
	}

	FScreenBoundGraphicsPipeline::~FScreenBoundGraphicsPipeline()
	{
		Destruct<Pipeline>();
	}

	void FScreenBoundGraphicsPipeline::Initialize(const FScreenBoundRenderTarget& renderTarget, const std::vector<ShaderDigest>& shaders, const Backend::GraphicsPipelineSpecification& spec)
	{
		GetAs<Pipeline>().Initialize(static_cast<Pipeline::RenderTargetType*>(renderTarget.GetBackendObject()), shaders, spec);
	}

	void FScreenBoundGraphicsPipeline::Terminate()
	{
		GetAs<Pipeline>().Terminate();
	}
}