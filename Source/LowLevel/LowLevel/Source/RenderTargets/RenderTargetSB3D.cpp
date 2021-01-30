// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LowLevel/RenderTargets/RenderTargetSB3D.h"
#include "Core/Backend/Interface/ScreenBoundRenderTarget.h"

namespace Flint
{
	namespace LowLevel
	{
		void RenderTargetSB3D::Initialize(const Device& device, Vector2 extent, UI32 bufferCount)
		{
			mHandle = Interface::CreateRenderTargetSB3D(device.GetHandle(), extent, bufferCount);
		}

		void RenderTargetSB3D::Terminate()
		{
			Interface::DestroyRenderTargetSB3D(mHandle);
		}
	}
}