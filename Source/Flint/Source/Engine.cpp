// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Engine.h"
#include "Core/ErrorHandler/Logger.h"

#include "VulkanBackend/VulkanInstance.h"

namespace Flint
{
	void Engine::Initialize(BackendAPI api, bool enableValidation, UI32 width, UI32 height, const char* pTitle)
	{
		InitializeInstance(api, enableValidation);
		InitializeDisplay(width, height, pTitle);
		InitializeDevice();
	}

	bool Engine::BeginUpdate()
	{
		pDisplay->Update();
		pRenderTarget->PrepareToDraw();

		return true;
	}

	void Engine::EndUpdate()
	{
		pRenderTarget->SubmitCommand();
	}

	void Engine::Terminate()
	{
		if (pRenderTarget)
		{
			pRenderTarget->Terminate();
			delete pRenderTarget;
		}

		TerminateDevice();
		TerminateDisplay();
		TerminateInstance();
	}

	void Engine::SwitchAPI(BackendAPI newAPI)
	{
	}

	void Engine::CreateRenderTarget(const Vector2& extent, UI32 bufferCount)
	{
		pRenderTarget = pDevice->CreateRenderTarget(Backend::RenderTargetType::SCREEN_BOUND, extent, bufferCount);
	}

	void Engine::PrepareRenderTargetToRender()
	{
		pRenderTarget->PrepareCommandBuffers();
	}

	SceneComponent Engine::CreateSceneComponent(const WireFrame& wireFrame, const std::vector<ShaderDigest>& shaders, const Backend::GraphicsPipelineSpecification& spec)
	{
		SceneComponent component = {};
		component.pPipeline = pRenderTarget->CreateGraphicsPipeline(shaders, spec);
		component.mWireFrame = wireFrame;

		return component;
	}

	void Engine::DestroySceneComponent(SceneComponent& sceneComponent)
	{
		sceneComponent.pPipeline->Terminate();
		delete sceneComponent.pPipeline;

		sceneComponent.mWireFrame.Clear();
	}

	RenderResource Engine::SubmitToDrawQueue(const SceneComponent& sceneComponent, const Backend::DynamicStateContainer& dynamicStates)
	{
		RenderResource resource = {};
		resource.mUniformBuffers = sceneComponent.pPipeline->CreateUniformBuffers();
		resource.pPipelineResource = sceneComponent.pPipeline->CreatePipelineResource();
		resource.pPipelineResource->RegisterUniformBuffers(resource.mUniformBuffers);
		resource.mReference = pRenderTarget->AddDrawEntry(sceneComponent.mWireFrame, sceneComponent.pPipeline, resource.pPipelineResource, dynamicStates);

		return resource;
	}
}