// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/GraphicsEngine.hpp"

namespace Flint
{
	GraphicsEngine::GraphicsEngine(bool enableValidation)
	{
		pInstance = CreateInstance(enableValidation);
	}

	GraphicsEngine::~GraphicsEngine()
	{
		pInstance->Terminate();
	}
}