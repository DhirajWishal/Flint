// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ImageBasedLighting.hpp"

namespace Flint
{
	ImageBasedLighting::ImageBasedLighting()
	{
		pGraphicsScene = mApplication.CreateGraphicsScene("Default", FBox2D(-1));

		mSkyBox.Initialize(&mApplication);
		pGraphicsScene->SubmitGameObject(&mSkyBox);

		while (pGraphicsScene->GetDisplay()->IsOpen())
		{
			mApplication.PrepareNewFrame();
			mApplication.UpdateGraphicsScenes();

			mApplication.DrawGraphicsScenes();
		}
	}
}

int main()
{
	Flint::ImageBasedLighting ibl = {};
	return 0;
}