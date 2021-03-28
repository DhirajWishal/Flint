// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Types\DataTypes.h"

#define FLINT_DEFINE_HANDLE(name)						enum class name : UI64 { INVALID = 0 }
#define FLINT_DEFINE_FUNCTION_POINTER(ret, name, ...)	inline ret (*name)(__VA_ARGS__) = nullptr

namespace Flint
{
	namespace Backend
	{
		FLINT_DEFINE_HANDLE(InstanceHandle);

		FLINT_DEFINE_FUNCTION_POINTER(InstanceHandle, CreateInstance, bool enableValidation);
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyInstance, InstanceHandle handle);
	}
}