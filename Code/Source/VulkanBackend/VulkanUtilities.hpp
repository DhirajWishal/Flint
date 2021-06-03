// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"
#include "Flint/Core/Error.hpp"

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