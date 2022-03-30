// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/GraphicsEngine.hpp"

namespace Flint
{
	GraphicsEngine::GraphicsEngine(bool enableValidation)
	{
		pInstance = std::make_shared<VulkanBackend::VulkanInstance>(enableValidation);
	}

	GraphicsEngine::~GraphicsEngine()
	{
		pInstance->Terminate();
	}

	std::shared_ptr<GraphicsDevice> GraphicsEngine::CreateGraphicsDevice()
	{
		return std::shared_ptr<GraphicsDevice>();
	}
}