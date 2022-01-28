// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

export module Flint.Core.Hasher;

import Flint.Core.DataType;

export namespace Flint
{
	namespace Hasher
	{
		/**
		 * Perform hash to a block of data.
		 * 
		 * @param pDataBlock The data block pointer.
		 * @param size The size of the block in bytes.
		 * @param seed The hash seed.
		 * @return The hashed value.
		 */
		uint64 HashDataBlock(const void* pDataBlock, const uint64 size, const uint64 seed = 0);
	}
}

module: private;

#define XXH_INLINE_ALL
import <xxhash.h>;

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