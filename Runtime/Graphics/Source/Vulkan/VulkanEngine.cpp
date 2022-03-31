// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/Vulkan/VulkanEngine.hpp"
#include "Graphics/Vulkan/VulkanReactor.hpp"

namespace Flint
{
	VulkanEngine::VulkanEngine(bool enableValidation)
		: GraphicsEngine(enableValidation), m_Instance(enableValidation)
	{
	}

	std::unique_ptr<GraphicsReactor> VulkanEngine::CreateReactor(DeviceFlags flags)
	{
		return std::make_unique<VulkanReactor>(this, flags);
	}
}