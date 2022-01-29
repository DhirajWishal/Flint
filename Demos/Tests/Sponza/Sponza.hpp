// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TestBase/Application.hpp"
#include "TestBase/DefaultProcessingPipeline.hpp"
#include "Object.hpp"

namespace Flint
{
	class Sponza
	{
	public:
		Sponza();

		Application mApplication = {};
		std::unique_ptr<DefaultProcessingPipeline> pProcessingPipeline = nullptr;
	};
}