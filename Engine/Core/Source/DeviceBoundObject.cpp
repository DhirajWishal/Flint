// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/DeviceBoundObject.hpp"
#include "Core/Errors/InvalidArgumentError.hpp"

namespace Flint
{

	DeviceBoundObject::DeviceBoundObject(const std::shared_ptr<Device>& pDevice)
		: m_pDevice(pDevice)
	{
		if (!pDevice)
			throw InvalidArgumentError("The device pointer should not be null!");
	}
}