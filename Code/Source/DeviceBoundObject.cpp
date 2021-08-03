// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DeviceBoundObject.hpp"

namespace Flint
{
	DeviceBoundObject::DeviceBoundObject(const std::shared_ptr<Device>& pDevice) : pDevice(pDevice)
	{
		if (!pDevice)
			FLINT_THROW_INVALID_ARGUMENT("Device pointer should not be null!");
	}
}