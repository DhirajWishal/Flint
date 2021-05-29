// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Core/DataTypes.h"

namespace Flint
{
	namespace Hasher
	{
		UI64 GenerateSeed64();
		UI64 Hash64(const void* pData, UI64 size, UI64 seed = 0);
		UI64 QuickHash64(const void* pData, UI64 size);
	}
}