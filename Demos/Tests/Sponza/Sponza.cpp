// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Sponza.hpp"

#include "TestBase/GraphicsScene.hpp"
#include "TestBase/Nodes/OffScreenPass.hpp"

#include <optick.h>

namespace Flint
{
	Sponza::Sponza()
	{
		OPTICK_EVENT();

		pGraphicsScene = mApplication.CreateGraphicsScene("Default", FBox2D(-1));

		// Create the processing pipeline.
		pProcessingPipeline = std::make_unique<ProcessingPipeline>(mApplication.GetDevice(), FBox2D(-1), "Flint: Sponza Test", 0, MultiSampleCount::One);
		auto pRenderTarget = pProcessingPipeline->CreateProcessingNode<OffScreenPass>();

		mObject.Initialize(&mApplication);
		pGraphicsScene->SubmitGameObject(&mObject);
		pGraphicsScene->mCamera.SetMovementBias(0.05f);

		while (pGraphicsScene->GetDisplay()->IsOpen())
		{
			mApplication.PrepareNewFrame();
			mApplication.UpdateGraphicsScenes();

			mApplication.DrawGraphicsScenes();

			// Execute the processing pipeline.
			pProcessingPipeline->Execute();
		}

		mApplication.Cleanup();
		mObject.Terminate();
	}
}

int main()
{
	Flint::Sponza sponza = {};
	return 0;
}