// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Device.hpp"
#include "VulkanInstance.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanDevice final : public Device
		{
		public:
			VulkanDevice(Instance& instance);

			virtual void Terminate() override final;
		};
	}
}