// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Core/Error.hpp"
#include <vulkan/vulkan.h>

namespace Flint
{
	namespace VulkanBackend
	{
		namespace Utilities
		{
			void CheckResult(VkResult result);
		}
	}
}