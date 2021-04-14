// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Objects\FScreenBoundRenderTarget.h"
#include "Flint\Objects\FGraphicsPipeline.h"

#if defined(FLINT_BACKEND_VULKAN)
#include "VulkanBackend\RenderTargets\VulkanScreenBoundRenderTargetS.h"
#include "VulkanBackend\VulkanBuffer.h"
typedef Flint::VulkanBackend::VulkanScreenBoundRenderTargetS RenderTarget;
typedef Flint::VulkanBackend::VulkanBuffer Buffer;

#elif defined(FLINT_BACKEND_DIRECT_X_12)

#elif defined(FLINT_BACKEND_WEB_GPU)

#else
error "Selected backend is invalid! Make sure that you select a proper supported backend!"

#endif // defined(FLINT_BACKEND_VULKAN)


namespace Flint
{
	FScreenBoundRenderTarget::FScreenBoundRenderTarget()
	{
		Construct<RenderTarget>();
	}

	FScreenBoundRenderTarget::~FScreenBoundRenderTarget()
	{
	}

	void FScreenBoundRenderTarget::Initialize(const FDevice& device, const FDisplay& display, UI64 bufferCount)
	{
		GetAs<RenderTarget>().Initialize(static_cast<RenderTarget::DeviceType*>(device.GetBackendObject().get()), static_cast<RenderTarget::DisplayType*>(display.GetBackendObject().get()), bufferCount);
	}

	void FScreenBoundRenderTarget::Terminate()
	{
		GetAs<RenderTarget>().Terminate();
	}

	void FScreenBoundRenderTarget::BakeCommands()
	{
		GetAs<RenderTarget>().BakeCommands();
	}

	void FScreenBoundRenderTarget::PrepareToDraw()
	{
		GetAs<RenderTarget>().PrepareToDraw();
	}

	void FScreenBoundRenderTarget::SubmitCommand()
	{
		GetAs<RenderTarget>().SubmitCommand();
	}

	UI64 FScreenBoundRenderTarget::GetFrameIndex() const
	{
		return GetAs<RenderTarget>().GetFrameIndex();
	}

	UI64 FScreenBoundRenderTarget::GetImageIndex() const
	{
		return GetAs<RenderTarget>().GetImageIndex();
	}

	FDrawIndex FScreenBoundRenderTarget::AddWireFrameToStaticDrawQueue(const WireFrame& wireFrame)
	{
		return FDrawIndex{ GetAs<RenderTarget>().AddStaticDrawEntry(static_cast<Buffer*>(wireFrame.mVertexBuffer.GetBackendObject().get()), static_cast<Buffer*>(wireFrame.mIndexBuffer.GetBackendObject().get())) };
	}

	void FScreenBoundRenderTarget::RemoveWireFrameFromStaticDrawQueue(const FDrawIndex index)
	{
		GetAs<RenderTarget>().RemoveStaticDrawEntry(static_cast<UI64>(index));
	}

	FDrawIndex FScreenBoundRenderTarget::AddWireFrameToDynamicDrawQueue(const WireFrame& wireFrame)
	{
		return FDrawIndex{ GetAs<RenderTarget>().AddDynamicDrawEntry(static_cast<Buffer*>(wireFrame.mVertexBuffer.GetBackendObject().get()), static_cast<Buffer*>(wireFrame.mIndexBuffer.GetBackendObject().get())) };
	}

	void FScreenBoundRenderTarget::RemoveWireFrameFromDynamicDrawQueue(const FDrawIndex index)
	{
		GetAs<RenderTarget>().RemoveDynamicDrawEntry(static_cast<UI64>(index));
	}

	FGraphicsPipeline FScreenBoundRenderTarget::CreateGraphicsPipeline(const std::vector<FShader>& shaders, const Backend::GraphicsPipelineSpecification& spec)
	{
		FGraphicsPipeline pipeline = {};
		pipeline.Initialize(*this, shaders, spec);

		return pipeline;
	}
}