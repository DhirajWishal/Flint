// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"

namespace Flint
{
	namespace Backend
	{
		FLINT_DEFINE_HANDLE(DeviceHandle);

		FLINT_DEFINE_FUNCTION_POINTER(DeviceHandle, CreateDevice, InstanceHandle instanceHandle);
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyDevice, DeviceHandle handle);
	}
}