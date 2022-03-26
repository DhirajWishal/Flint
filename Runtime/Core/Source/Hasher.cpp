// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Hasher.hpp"

#define XXH_INLINE_ALL
#include <xxhash.h>

namespace Flint
{
	namespace Hasher
	{
		uint64 HashDataBlock(const void* pDataBlock, const uint64 size, const uint64 seed)
		{
			return static_cast<uint64>(XXH64(pDataBlock, static_cast<size_t>(size), static_cast<XXH64_hash_t>(seed)));
		}
	}
}