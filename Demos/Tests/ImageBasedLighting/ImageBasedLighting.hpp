// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TestBase/Application.hpp"
#include "TestBase/GraphicsScene.hpp"

#include "Components/SkyBox.hpp"

namespace Flint
{
	class ImageBasedLighting
	{
	public:
		ImageBasedLighting();

		Application mApplication = {};
		GraphicsScene* pGraphicsScene = nullptr;

		SkyBox mSkyBox = {};
	};
}
