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
		mDisplay.Update();
		mRenderTarget.PrepareToDraw();

		return true;
	}

	void Engine::EndUpdate()
	{
		mRenderTarget.SubmitCommand();
	}

	void Engine::Terminate()
	{
		mRenderTarget.Terminate();

		TerminateDevice();
		TerminateDisplay();
		TerminateInstance();
	}

	void Engine::SwitchAPI(BackendAPI newAPI)
	{
	}

	void Engine::CreateRenderTarget(UI32 bufferCount)
	{
		mRenderTarget.Initialize(mDevice, mDisplay, bufferCount);
	}

	void Engine::PrepareRenderTargetToRender()
	{
		mRenderTarget.BakeCommands();
	}

	void Engine::SetupSceneComponent(SceneComponent& sceneComponent, const WireFrame& wireFrame, const std::vector<ShaderDigest>& shaders, const Backend::GraphicsPipelineSpecification& spec)
	{
		sceneComponent.mPipeline.Initialize(mRenderTarget, shaders, spec);
		sceneComponent.mWireFrame = wireFrame;

		sceneComponent.mDrawID = sceneComponent.mPipeline.AddStaticDrawEntry(&sceneComponent.mWireFrame.mVertexBuffer, &sceneComponent.mWireFrame.mIndexBuffer);
		mRenderTarget.RegisterGraphicsPipelineStatic(&sceneComponent.mPipeline);
	}

	void Engine::DestroySceneComponent(SceneComponent& sceneComponent)
	{
		sceneComponent.mPipeline.Terminate();
		sceneComponent.mWireFrame.Clear();
	}

	void Engine::SubmitToDrawQueue(RenderResource& renderResource, SceneComponent& sceneComponent, const Backend::DynamicStateContainer& dynamicStates)
	{
		renderResource.mUniformBuffers = sceneComponent.mPipeline.CreateUniformBufferContainer();
		renderResource.mPipelineResource = sceneComponent.mPipeline.CreatePipelineResource();
		renderResource.mPipelineResource.RegisterUniformBufferContainer(renderResource.mUniformBuffers);
		for (auto& data : sceneComponent.mWireFrame.mDrawData)
			INSERT_INTO_VECTOR(renderResource.mDrawDataIDs, sceneComponent.mPipeline.AddStaticDrawData(sceneComponent.mDrawID, data.mVertexCount, data.mVertexOffset, data.mIndexCount, data.mIndexOffset, &renderResource.mPipelineResource, dynamicStates));
	}
}