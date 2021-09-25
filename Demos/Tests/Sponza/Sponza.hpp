// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TestBase/Application.hpp"
#include "Object.hpp"

namespace Flint
{
	class Sponza
	{
	public:
		Sponza();

		Application mApplication = {};
		GraphicsScene* pGraphicsScene = nullptr;

		Object mObject = {};
	};
}