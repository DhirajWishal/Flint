// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanEngine.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanEngine::VulkanEngine(VulkanInstance& instance)
			: EngineCRTP(instance)
		{
			// Select a physical device.
			selectPhysicalDevice();
		}

		VulkanEngine::~VulkanEngine()
		{
		}

		void VulkanEngine::selectPhysicalDevice()
		{

		}
	}
}