// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Device.hpp"

namespace Flint
{
	Device::Device(const std::shared_ptr<Instance>& pInstance, DeviceFlags flags) : pInstance(pInstance), mFlags(flags)
	{
		if (!pInstance)
			FLINT_THROW_INVALID_ARGUMENT("Instance pointer should not be null!");
	}
}