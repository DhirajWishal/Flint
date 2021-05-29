// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core\Hasher\Hasher.h"

#include <xxhash.h>
#include <random>

namespace Flint
{
	namespace Hasher
	{
		UI64 GenerateSeed64()
		{
			return std::mt19937_64()();
		}

		UI64 Hash64(const void* pData, UI64 size, UI64 seed)
		{
			return XXH64(pData, size, seed);
		}

		UI64 QuickHash64(const void* pData, UI64 size)
		{
			return XXH3_64bits(pData, size);
		}
	}
}