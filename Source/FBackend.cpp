// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/FBackend.h"

#include "VulkanBackend/VulkanBackend.h"

namespace Flint
{
	FBackend* CreateBackend(BackendAPI api, bool enableValidation)
	{
		return new VulkanBackend::VulkanBackend(enableValidation);
	}
}