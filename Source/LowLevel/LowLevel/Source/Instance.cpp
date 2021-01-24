// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LowLevel/Instance.h"
#include "Core/Backend/Interface/Instance.h"

namespace Flint
{
	namespace LowLevel
	{
		void Instance::Initialize(bool enableValidation)
		{
			mHandle = Interface::CreateInstance(enableValidation);
		}

		void Instance::Terminate()
		{
			Interface::DestroyInstance(mHandle);
		}
	}
}