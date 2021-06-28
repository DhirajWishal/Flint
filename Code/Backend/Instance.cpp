// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Instance.hpp"

#include "VulkanBackend/VulkanInstance.hpp"

namespace Flint
{
	namespace Backend
	{
		Instance& CreateInstance(bool enableValidation)
		{
			return *new VulkanBackend::VulkanInstance(enableValidation);
		}

		void DestroyInstance(Instance& instance)
		{
			instance.Terminate();
			delete& instance;
		}
	}
}