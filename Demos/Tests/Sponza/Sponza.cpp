// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "TestBase/GraphicsScene.hpp"
#include "TestBase/Nodes/FXAAPass.hpp"
#include "TestBase/Nodes/ScreenSpaceReflectionPass.hpp"
#include "TestBase/Nodes/ImGuiPass.hpp"

#include "TestBase/Application.hpp"
#include "TestBase/DefaultProcessingPipeline.hpp"

#include <optick.h>

export module Sponza;
import Object;

namespace Flint
{
	export class Sponza
	{
	public:
		Sponza();

		Application mApplication = {};
		std::unique_ptr<DefaultProcessingPipeline> pProcessingPipeline = nullptr;
	};

	Sponza::Sponza()
	{
		OPTICK_EVENT();

		// Create the processing pipeline.
		pProcessingPipeline = std::make_unique<DefaultProcessingPipeline>(mApplication.GetDevice(), FBox2D(-1), "Flint: Sponza Test", 0, MultiSampleCount::One);

		// Create all the render passes.
		const auto pRenderTarget = pProcessingPipeline->CreateProcessingNode<OffScreenPass>();
		const auto pRenderTargetSSR = pProcessingPipeline->CreateProcessingNode<ScreenSpaceReflectionPass>(pRenderTarget.get());
		const auto pRenderTargetFXAA = pProcessingPipeline->CreateProcessingNode<FXAAPass>(pRenderTarget.get());
		const auto pRenderTargetImGui = pProcessingPipeline->CreateProcessingNode<ImGuiPass>();

		const auto pObject = pRenderTarget->CreateGameObject<Object>(&mApplication, pRenderTarget);

		while (pProcessingPipeline->GetDisplay()->IsOpen())
		{
			// Prepare the new frame.
			mApplication.PrepareNewFrame();
			pRenderTargetFXAA->DrawUi();

			// Update the pipeline.
			const uint64 delta = pProcessingPipeline->Update();

			// Update all the game objects.
			for (const auto& pGameObjects : pRenderTarget->GetGameObjects())
				pGameObjects->Update(delta, &pProcessingPipeline->GetCamera());

			// End the frame.
			mApplication.EndFrame();

			// Execute the processing pipeline.
			pProcessingPipeline->Execute();
		}

		mApplication.Cleanup();
		pObject->Terminate();
	}
}