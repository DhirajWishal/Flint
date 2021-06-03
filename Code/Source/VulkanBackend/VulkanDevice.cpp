// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanDevice::VulkanDevice(Instance& instance, DeviceFlags flags) : Device(instance, flags)
		{
		}

		void VulkanDevice::Terminate()
		{
		}
	}
}