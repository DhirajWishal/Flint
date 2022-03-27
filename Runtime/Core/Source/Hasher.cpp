// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Hasher.hpp"

#define XXH_INLINE_ALL
#include <xxhash.h>

namespace Flint
{
	namespace Hasher
	{
		uint64_t HashDataBlock(const void* pDataBlock, const uint64_t size, const uint64_t seed)
		{
			return static_cast<uint64_t>(XXH64(pDataBlock, static_cast<size_t>(size), static_cast<XXH64_hash_t>(seed)));
		}
	}
}