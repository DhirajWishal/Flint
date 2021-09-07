// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Hasher.hpp"

#define XXH_INLINE_ALL
#include <xxhash.h>

namespace Flint
{
	namespace Hasher
	{
		UI64 HashDataBlock(const void* pDataBlock, const UI64 size, const UI64 seed)
		{
			return static_cast<UI64>(XXH64(pDataBlock, static_cast<size_t>(size), static_cast<XXH64_hash_t>(seed)));
		}
	}
}