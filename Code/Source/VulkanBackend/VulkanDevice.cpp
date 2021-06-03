// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanDevice::VulkanDevice(Instance& instance) : Device(instance)
		{
		}

		void VulkanDevice::Terminate()
		{
		}
	}
}