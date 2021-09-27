// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Sponza.hpp"

#include "TestBase/GraphicsScene.hpp"

#include <optick.h>

namespace Flint
{
	Sponza::Sponza()
	{
		OPTICK_EVENT();

		pGraphicsScene = mApplication.CreateGraphicsScene("Default", FBox2D(-1));

		mObject.Initialize(&mApplication);
		pGraphicsScene->SubmitGameObject(&mObject);
		pGraphicsScene->mCamera.SetMovementBias(0.05f);

		while (pGraphicsScene->GetDisplay()->IsOpen())
		{
			mApplication.PrepareNewFrame();
			mApplication.UpdateGraphicsScenes();

			mApplication.DrawGraphicsScenes();
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