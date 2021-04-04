// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Objects\FScreenBoundRenderTarget.h"
#include "Flint\Objects\FScreenBoundGraphicsPipeline.h"

#include "VulkanBackend\RenderTargets\VulkanScreenBoundRenderTargetS.h"
typedef Flint::VulkanBackend::VulkanScreenBoundRenderTargetS RenderTarget;

namespace Flint
{
	constexpr const UI64 GetScreenBoundRenderTargetSize()
	{
		return sizeof(RenderTarget);
	}

	constexpr const UI64 GetScreenBoundRenderTargetAllignment()
	{
		return alignof(RenderTarget);
	}

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