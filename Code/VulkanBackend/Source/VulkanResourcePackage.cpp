// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanResourcePackage.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanResourcePackage::VulkanResourcePackage(const UI32 setIndex, const std::vector<UI32>& bufferBindings, const std::vector<UI32>& imageBindings)
			: ResourcePackage(setIndex, bufferBindings, imageBindings)
		{
		}
	}
}