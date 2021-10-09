// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Sponza.hpp"

#include "TestBase/GraphicsScene.hpp"
#include "TestBase/Nodes/FXAAPass.hpp"
#include "TestBase/Nodes/ScreenSpaceReflectionPass.hpp"

#include <optick.h>

namespace Flint
{
	Sponza::Sponza()
	{
		OPTICK_EVENT();

		// Create the processing pipeline.
		pProcessingPipeline = std::make_unique<DefaultProcessingPipeline>(mApplication.GetDevice(), FBox2D(-1), "Flint: Sponza Test", 0, MultiSampleCount::One);
		auto pRenderTarget = pProcessingPipeline->CreateProcessingNode<OffScreenPass>();
		pRenderTarget->CreateGameObject<Object>(&mApplication, pRenderTarget);

		//auto pRenderTargetFXAA = pProcessingPipeline->CreateProcessingNode<FXAAPass>(pRenderTarget.get());
		auto pRenderTargetSSR = pProcessingPipeline->CreateProcessingNode<ScreenSpaceReflectionPass>(pRenderTarget.get());
		while (pProcessingPipeline->GetDisplay()->IsOpen())
		{
			// Prepare the new frame.
			mApplication.PrepareNewFrame();
			//pRenderTargetFXAA->DrawUi();

			// Update the pipeline.
			const UI64 delta = pProcessingPipeline->Update();

			// Update all the game objects.
			for (auto const& pGameObjects : pRenderTarget->GetGameObjects())
				pGameObjects->Update(delta, &pProcessingPipeline->GetCamera());

			// End the frame.
			mApplication.EndFrame();

			// Execute the processing pipeline.
			pProcessingPipeline->Execute();
		}

		mApplication.Cleanup();
	}
}

int main()
{
	Flint::Sponza sponza = {};
	return 0;
}