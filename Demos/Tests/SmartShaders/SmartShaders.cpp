// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "SmartShaders.hpp"

#include "TestBase/GraphicsScene.hpp"

namespace Flint
{
	SmartShaders::SmartShaders()
	{
		pGraphicsScene = mApplication.CreateGraphicsScene("Default", FBox2D(-1));

		//mObject.Initialize(&mApplication);
		//pGraphicsScene->SubmitGameObject(&mObject);
		pGraphicsScene->mCamera.SetMovementBias(0.05f);

		while (pGraphicsScene->GetDisplay()->IsOpen())
		{
			mApplication.PrepareNewFrame();
			mApplication.UpdateGraphicsScenes();

			mApplication.DrawGraphicsScenes();
		}

		mApplication.Cleanup();
		//mObject.Terminate();
	}
}

int main()
{
	Flint::SmartShaders sponza = {};
	return 0;
}