// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Backend/Interface.h"

namespace Flint
{
	namespace Interface
	{
		bool IsInitialized()
		{
			return CreateInstance && DestroyInstance
				&& CreateDisplay && DestroyDisplay && GetInputCenter
				&& CreateDevice && DestroyDevice;
		}
	}
}