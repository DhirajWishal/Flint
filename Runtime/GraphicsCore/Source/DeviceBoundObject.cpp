// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/DeviceBoundObject.hpp"

namespace Flint
{
	DeviceBoundObject::DeviceBoundObject(Device* pDevice) : pDevice(pDevice)
	{
		if (!pDevice)
			throw std::invalid_argument("Device pointer should not be null!");
	}
}