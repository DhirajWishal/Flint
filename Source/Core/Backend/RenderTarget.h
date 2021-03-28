// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "Display.h"

namespace Flint
{
	namespace Backend
	{
		FLINT_DEFINE_HANDLE(RenderTargetHandle);

		FLINT_DEFINE_FUNCTION_POINTER(RenderTargetHandle, CreateRenderTargetSB, DeviceHandle deviceHandle, DisplayHandle displayHandle, UI32 bufferCount);
		FLINT_DEFINE_FUNCTION_POINTER(RenderTargetHandle, CreateRenderTargetOS, DeviceHandle deviceHandle, const Vector2 extent, UI32 bufferCount);

		FLINT_DEFINE_FUNCTION_POINTER(void, DestroyRenderTarget, RenderTargetHandle handle);
	}
}