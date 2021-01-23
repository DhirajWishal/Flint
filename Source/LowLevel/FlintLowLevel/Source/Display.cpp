// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "FlintLowLevel/Display.h"
#include "Core/Backend/Interface/Display.h"

namespace Flint
{
	namespace LowLevel
	{
		void Display::Initialize(const Instance& instance, UI32 width, UI32 height, const char* pTitle)
		{
			mHandle = Interface::CreateDisplay(instance.GetHandle(), width, height, pTitle);
			pInputCenter = Interface::GetInputCenter(mHandle);
		}

		void Display::Terminate()
		{
			Interface::DestroyDisplay(mHandle);
		}
	}
}