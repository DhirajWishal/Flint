// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LowLevel/Device.h"
#include "Core/Backend/Interface/Device.h"

namespace Flint
{
	namespace LowLevel
	{
		void Device::Initialize(const Display& display)
		{
			mHandle = Interface::CreateDevice(display.GetHandle());
		}

		void Device::Terminate()
		{
			Interface::DestroyDevice(mHandle);
		}
	}
}