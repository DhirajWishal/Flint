// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LowLevel/RenderTargets/RenderTargetSB2D.h"
#include "Core/Backend/Interface/RenderTarget.h"

namespace Flint
{
	namespace LowLevel
	{
		void RenderTargetSB2D::Initialize(const Device& device, Vector2 extent, UI32 bufferCount)
		{
			mHandle = Interface::CreateRenderTargetSB2D(device.GetHandle(), extent, bufferCount);
		}

		void RenderTargetSB2D::Terminate()
		{
			Interface::DestroyRenderTargetSB2D(mHandle);
		}
	}
}