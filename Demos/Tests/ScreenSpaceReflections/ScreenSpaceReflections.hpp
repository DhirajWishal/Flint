// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TestBase/Application.hpp"

namespace Flint
{
	class ScreenSpaceReflections final : public Application
	{
	public:
		ScreenSpaceReflections();

		Application mApplication = {};
		GraphicsScene* pGraphicsScene = nullptr;
	};
}