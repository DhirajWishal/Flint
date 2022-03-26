// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/Device.hpp"

namespace Flint
{
	Device::Device(const std::shared_ptr<Instance>& pInstance, const DeviceFlags flags) : pInstance(pInstance), mFlags(flags)
	{
		if (!pInstance)
			throw std::invalid_argument("Instance pointer should not be null!");
	}
}