// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Objects\FScreenBoundRenderTarget.h"
#include "Flint\Objects\FScreenBoundGraphicsPipeline.h"

#if defined(FLINT_BACKEND_VULKAN)
#include "VulkanBackend\RenderTargets\VulkanScreenBoundRenderTargetS.h"
typedef Flint::VulkanBackend::VulkanScreenBoundRenderTargetS RenderTarget;

#endif // defined(FLINT_BACKEND_VULKAN)


namespace Flint
{
	FScreenBoundRenderTarget::FScreenBoundRenderTarget()
	{
		Construct<RenderTarget>();
	}

	FScreenBoundRenderTarget::~FScreenBoundRenderTarget()
	{
		Destruct<RenderTarget>();
	}

	void FScreenBoundRenderTarget::Initialize(const FDevice& device, const FDisplay& display, UI64 bufferCount)
	{
		GetAs<RenderTarget>().Initialize(static_cast<RenderTarget::DeviceType*>(device.GetBackendObject()), static_cast<RenderTarget::DisplayType*>(display.GetBackendObject()), bufferCount);
	}

	void FScreenBoundRenderTarget::Terminate()
	{
		GetAs<RenderTarget>().Terminate();
	}
	
	FScreenBoundGraphicsPipeline FScreenBoundRenderTarget::CreateGraphicsPipeline(const std::vector<ShaderDigest>& shaders, const Backend::GraphicsPipelineSpecification& spec)
	{
		FScreenBoundGraphicsPipeline pipeline = {};
		pipeline.Initialize(*this, shaders, spec);

		return pipeline;
	}
}