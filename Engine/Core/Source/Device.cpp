// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Device.hpp"
#include "Core/Errors/InvalidArgumentError.hpp"

namespace Flint
{

	Device::Device(const std::shared_ptr<Instance>& pInstance)
		: m_pInstance(pInstance)
	{
		if (!pInstance)
			throw InvalidArgumentError("The instance pointer should not be null!");
	}
}