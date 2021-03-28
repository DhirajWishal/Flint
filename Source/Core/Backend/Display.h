// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"
#include "Core\Maths\Vector\Vector2.h"
#include "Core\Inputs\InputCenter.h"

namespace Flint
{
	namespace Backend
	{
		FLINT_DEFINE_HANDLE(DisplayHandle);

		FLINT_DEFINE_FUNCTION_POINTER(DisplayHandle, CreateDisplay, InstanceHandle instanceHandle, const Vector2 extent, const char* pTitle);
		FLINT_DEFINE_FUNCTION_POINTER(void, UpdateDisplay, DisplayHandle handle);
		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyDisplay, DisplayHandle handle);

		FLINT_DEFINE_FUNCTION_POINTER(Inputs::InputCenter*, GetDisplayInputCenter, DisplayHandle handle);
	}
}