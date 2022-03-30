// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/Vulkan/VulkanEngine.hpp"

namespace Flint
{
	VulkanEngine::VulkanEngine(bool enableValidation)
		: m_Instance(enableValidation)
	{
	}
}