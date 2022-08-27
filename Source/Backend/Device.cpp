// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Backend/Device.hpp"
#include "Flint/Core/Errors/InvalidArgumentError.hpp"

namespace Flint
{
	namespace Backend
	{
		Device::Device(const std::shared_ptr<Instance>& pInstance)
			: m_pInstance(pInstance)
		{
			if (!pInstance)
				throw InvalidArgumentError("The instance pointer should not be null!");
		}
	}
}